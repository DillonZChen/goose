import logging
import sqlite3
import time
from typing import Any, Union

from pddl.action import Action
from pddl.custom_types import name
from pddl.logic import Constant, Predicate
from pddl.logic.base import Not
from pddl.logic.functions import Divide, EqualTo, Minus, NumericFunction, NumericValue, Plus, Times
from pddl.logic.predicates import EqualTo as ObjectEqualTo
from succgen.planning import Literal, PDDLState, get_condition_list
from succgen.planning.action import SGAction
from succgen.planning.state import SGState
from succgen.planning.strings import NumericCondition, get_numeric_condition_symbol
from succgen.planning.task import SGTask
from succgen.util.logging import mat_to_str
from succgen.util.managers import TimerContextManager


_PARAM = "x__"
_DUMMY_TERM = "z__"
_DUMMY_CONS = -1
_NUMERIC_VALUE_TERM = "v__"

_OBJ_TYPE = "INTEGER"  # objects are indexed
_VAL_TYPE = "REAL"


def to_table_name(input: Union[NumericFunction, Predicate, name], desc: str = "") -> str:
    """Convert a predicatem function or object type name to a valid SQL name.

    We do this because SQLite has some illegal characters and names, e.g. `at` predicate, and hyphens in names
    """
    if isinstance(input, NumericFunction | Predicate):
        ret = input.name
    else:
        ret = input
    if isinstance(input, NumericFunction) or desc == "function":
        suffix = "n"
    elif isinstance(input, Predicate) or desc == "predicate":
        suffix = "p"
    elif isinstance(input, name) or desc == "type":
        suffix = "t"
    else:
        raise ValueError(f"Invalid predicate type: {type(input)}")
    ret = f"{ret.replace('-', '_')}__{suffix}"
    return ret


class SQLiteApplicableActionGenerator:
    def __init__(self, task: SGTask, debug: bool = False) -> None:
        self._debug = debug

        self._task = task
        self._domain = task.domain
        self._problem = task.problem

        # Domain information
        self._predicates = task.domain.predicates
        self._functions = task.domain.functions

        self._statics: PDDLState = frozenset()  # populated by self._initialise_statics()

        # SQLite objects
        self._con = sqlite3.connect(":memory:")
        self._cur = self._con.cursor()

        # Data structures
        self._pred_i_to_table = [to_table_name(p, desc="predicate") for p in self._task.i_to_pred]
        self._func_i_to_table = [to_table_name(f, desc="function") for f in self._task.i_to_func]

        # Queries
        self._schema_queries: list[str] = []
        self._clear_queries: list[str] = []
        self._insert_queries_p: list[tuple[str, str]] = [_ for _ in range(len(self._predicates))]
        self._insert_queries_f: list[tuple[int, int, str]] = []

        # Initialise
        with TimerContextManager("preprocessing PDDL information for SQLite"):
            self._initialise_from_domain()
            self._initialise_from_problem()

        # Timers
        self._t_aag_processing = 0
        self._t_aag_execution = 0
        self._profiling = {
            "clear": 0,
            "insert_fact": 0,
            "insert_value": 0,
            "query": 0,
        }

    @property
    def t_aag_processing(self) -> float:
        return self._t_aag_processing

    @property
    def t_aag_execution(self) -> float:
        return self._t_aag_execution

    def _initialise_from_domain(self) -> None:
        domain = self._domain

        """ Set up tables """
        predicate_arg_to_term: dict[tuple[str, int], str] = {}

        # Predicates
        for literal in domain.predicates:
            sql_terms = self._to_sql_terms(literal, constants=False)
            table_terms = self._sql_terms_str(literal, constants=False, sql_typing=True)
            index_terms = self._sql_terms_str(literal, constants=False, sql_typing=False)
            table_name = to_table_name(literal)
            self._exec_dev(f"CREATE TABLE {table_name} ({table_terms})")
            self._exec_dev(f"CREATE INDEX idx_{table_name} ON {table_name}({index_terms})")
            for i, term in enumerate(sql_terms):
                predicate_arg_to_term[(table_name, i)] = term

        # Numeric Functions
        for function in domain.functions:
            sql_terms = self._to_sql_terms(function, constants=False)
            table_terms = self._sql_terms_str(function, constants=False, sql_typing=True)
            index_terms = self._sql_terms_str(function, constants=False, sql_typing=False)
            table_name = to_table_name(function)
            self._exec_dev(f"CREATE TABLE {table_name} ({table_terms})")
            self._exec_dev(f"CREATE INDEX idx_{table_name} ON {table_name}({index_terms})")
            for i, term in enumerate(sql_terms):
                predicate_arg_to_term[(table_name, i)] = term

        # Object Types
        domain_types = set(domain.types.keys()) | set(domain.types.values()) | {"object"}
        domain_types -= {None}
        for obj_type in domain_types:
            table_name = to_table_name(obj_type, desc="type")
            self._exec_dev(f"CREATE TABLE {table_name} ({_PARAM} {_OBJ_TYPE})")
            self._exec_dev(f"CREATE INDEX idx_{table_name} ON {table_name}({_PARAM})")
            predicate_arg_to_term[(table_name, 0)] = _PARAM

        """ Set up action queries """
        for action in sorted(domain.actions, key=lambda x: x.name):
            select_line = []
            from_line = []
            where_line = []

            # action parameters
            pddl_to_sql_term = {}
            for i, term in enumerate(action.parameters):
                assert not isinstance(term, Constant)
                pddl_to_sql_term[term.name] = f"t{i}.{_PARAM}"
                assert len(term.type_tags) <= 1
                if len(term.type_tags) == 0:
                    type_tag = "object"
                else:
                    type_tag = next(iter(term.type_tags))
                select_line.append(f"t{i}.{_PARAM}")
                from_line.append(f"{to_table_name(type_tag, desc='type')} t{i}")

            # action preconditions
            _symbol_i = 0

            def add_symbol(condition: Union[Predicate, NumericFunction]) -> str:
                nonlocal _symbol_i
                assert isinstance(condition, (Predicate, NumericFunction))
                table_name = to_table_name(condition)
                symbol_name = f"p{_symbol_i}"
                from_line.append(f"{table_name} {symbol_name}")
                _symbol_i += 1
                return symbol_name

            def unify_terms(
                condition: Union[Predicate, NumericCondition], symbol_name: str, output: list[str]
            ) -> None:
                terms = condition.terms
                if len(terms) == 0 and isinstance(condition, Predicate):
                    output.append(f"{symbol_name}.{_DUMMY_TERM} == {_DUMMY_CONS}")
                    return
                for i, term in enumerate(terms):
                    lhs = f"{symbol_name}.{predicate_arg_to_term[(to_table_name(condition), i)]}"
                    if isinstance(term, Constant):
                        rhs = self._task.obj_to_i[term.name]
                    else:
                        rhs = pddl_to_sql_term[term.name]
                    output.append(f"{lhs} == {rhs}")
                return

            def get_expr(expr) -> str:
                if isinstance(expr, NumericFunction):
                    symbol_name = add_symbol(expr)
                    unify_terms(expr, symbol_name, where_line)
                    return f"{symbol_name}.{_NUMERIC_VALUE_TERM}"
                elif isinstance(expr, NumericValue):
                    return str(expr.value)
                elif isinstance(expr, Plus):
                    assert len(expr.operands) == 2
                    return f"({get_expr(expr.operands[0])} + {get_expr(expr.operands[1])})"
                elif isinstance(expr, Minus):
                    assert len(expr.operands) == 2
                    return f"({get_expr(expr.operands[0])} - {get_expr(expr.operands[1])})"
                elif isinstance(expr, Times):
                    assert len(expr.operands) == 2
                    return f"({get_expr(expr.operands[0])} * {get_expr(expr.operands[1])})"
                elif isinstance(expr, Divide):
                    assert len(expr.operands) == 2
                    return f"({get_expr(expr.operands[0])} / {get_expr(expr.operands[1])})"
                else:
                    raise NotImplementedError(f"Unsupported expression {expr} of type {type(expr)}")

            for condition in get_condition_list(action.precondition):
                # TODO something about nested conditions e.g. AND of ORs
                if isinstance(condition, Predicate):
                    symbol_name = add_symbol(condition)
                    unify_terms(condition, symbol_name, where_line)
                elif isinstance(condition, NumericCondition):
                    comparison = get_numeric_condition_symbol(condition)
                    assert len(condition.operands) == 2
                    sql_terms = [get_expr(condition.operands[i]) for i in [0, 1]]
                    where_line.append(f"{sql_terms[0]} {comparison} {sql_terms[1]}")
                elif isinstance(condition, Not) and isinstance(condition.argument, Predicate):
                    n_condition = condition.argument
                    symbol_name = add_symbol(n_condition)
                    neg_table = []
                    unify_terms(n_condition, symbol_name, neg_table)
                    neg_table = " AND ".join(neg_table)
                    neg = f"NOT EXISTS (SELECT 1 FROM {to_table_name(n_condition)} {symbol_name} WHERE {neg_table})"
                    where_line.append(neg)
                elif isinstance(condition, Not) and isinstance(condition.argument, ObjectEqualTo):
                    lhs = pddl_to_sql_term[condition.argument.left.name]
                    rhs = pddl_to_sql_term[condition.argument.right.name]
                    where_line.append(f"{lhs} != {rhs}")
                else:
                    raise NotImplementedError(f"Unsupported condition {condition}")

            if len(select_line) == 0:
                select_line = "SELECT DISTINCT 1"  # for actions with no free variables
            else:
                select_line = "SELECT DISTINCT " + ", ".join(select_line)
            from_line = "FROM " + ", ".join(from_line)
            where_line = "WHERE " + " AND ".join(where_line)

            sql_query = "\n".join([select_line, from_line, where_line])
            logging.debug(action)
            logging.debug(sql_query)

            self._schema_queries.append(sql_query)

        """ Set up clear queries """
        for i in self._task.fluent_predicates:
            self._clear_queries.append(f"DELETE FROM {self._pred_i_to_table[i]}")
        for i in self._task.fluent_functions:
            self._clear_queries.append(f"DELETE FROM {self._func_i_to_table[i]}")

        """ Set up insert queries """
        for predicate in self._predicates:
            pname = predicate.name
            i = self._task.pred_to_i[pname]
            qs = ", ".join(["?"] * predicate.arity)
            if len(qs) == 0:  # for nullary predicates
                qs = _DUMMY_CONS
            self._insert_queries_p[i] = f"INSERT INTO {self._pred_i_to_table[i]} VALUES ({qs})"
        f_queries = {}
        for fluent in self._task.i_to_num_fluent:
            fname = fluent.name
            i = self._task.func_to_i[fname]
            values = ", ".join([str(self._task.obj_to_i[term.name]) for term in fluent.terms] + ["?"])
            header = f"INSERT INTO {self._func_i_to_table[i]} VALUES "
            if header not in f_queries:
                f_queries[header] = []
            f_queries[header].append(f"({values})")
        start = 0
        for header, queries in f_queries.items():
            query = header + ", ".join(queries)
            end = start + len(queries)
            self._insert_queries_f.append((start, end, query))
            start = end

    def _initialise_from_problem(self) -> None:
        problem = self._problem

        """ Add statics to database """
        for fact in self._task.statics:
            if isinstance(fact, Predicate):
                values = self._sql_terms_str(fact, constants=True)
                self._exec_dev(f"INSERT INTO {to_table_name(fact)} VALUES ({values})")
            elif isinstance(fact, NumericFunction):
                function = fact
                value = self._task.statics[fact]
                table = to_table_name(function)
                values = ", ".join(self._to_sql_terms(function, constants=True) + [str(value)])
                self._exec_dev(f"INSERT INTO {table} VALUES ({values})")
            else:
                raise ValueError(f"Unknown fact {fact} of type {type(fact)}")

        """ Add object typing to database """
        for obj in problem.objects | self._domain.constants:
            types = set(obj.type_tags)
            if len(types) == 0:
                types.add("object")
            added = set()
            for obj_type in types:
                while obj_type is not None and obj_type not in added:
                    table_name = to_table_name(obj_type, desc="type")
                    self._exec_dev(f"INSERT INTO {table_name} VALUES ('{self._task.obj_to_i[obj.name]}')")
                    if obj_type not in self._domain.types:
                        break
                    obj_type = self._domain.types[obj_type]

    def _exec_dev(self, cmd: str) -> Any:
        logging.debug(f"Executing sqlite cmd:\n{cmd}\n")
        try:
            return self._cur.execute(cmd)
        except sqlite3.Error as e:
            logging.error(f"Error executing sqlite cmd:\n\n{cmd}\n\nCausing the error:\n\n{e}")
            raise e

    def _to_sql_terms(self, input, constants: bool, sql_typing: bool = False) -> list[str]:
        assert isinstance(input, (Literal, NumericFunction, NumericCondition))

        if isinstance(input, Not):
            input = input.argument
        elif isinstance(input, NumericCondition):
            input = input.operands[0]

        terms = [t.name for t in input.terms]

        if len(terms) == 0 and isinstance(input, Predicate):
            terms.append(_DUMMY_TERM)
        elif isinstance(input, NumericFunction) and not constants:
            terms.append(_NUMERIC_VALUE_TERM)

        if constants:
            terms = [f"'{self._task.obj_to_i[term]}'" for term in terms]
        else:
            extended_terms = []
            for term in terms:
                if term in {_DUMMY_TERM, _NUMERIC_VALUE_TERM}:
                    extended_terms.append(term)
                else:
                    extended_terms.append(f"{term}_")
            terms = extended_terms

        if sql_typing:
            terms = [f"{term} {_OBJ_TYPE}" for term in terms]
            if isinstance(input, NumericFunction):
                terms[-1] = terms[-1].replace(f" {_OBJ_TYPE}", f" {_VAL_TYPE}")

        return terms

    def _sql_terms_str(self, literal: Union[Literal, Action], constants: bool, sql_typing: bool = False) -> str:
        terms = self._to_sql_terms(literal, constants, sql_typing=sql_typing)
        ret = f"{', '.join(terms)}"
        return ret

    def _clear_state(self) -> None:
        for q in self._clear_queries:
            self._cur.execute(q)

    def _insert_state(self, state: SGState) -> None:
        # insert fluents
        # tt = time.perf_counter()
        # queries = [[] for _ in range(len(self._insert_queries_p))]
        for fact in state.atoms:
            fact = self._task.atom_packer.unpack(fact)
            self._cur.execute(self._insert_queries_p[fact[0]], fact[1])
        #     queries[fact[0]].append(fact[1])
        # for i, query in enumerate(self._insert_queries_p):
        #     if len(queries[i]) == 0:
        #         continue
        #     self._cur.executemany(query, queries[i])
        # breakpoint()
        # self._profiling["insert_fact"] += time.perf_counter() - tt
        tt = time.perf_counter()
        for start, end, query in self._insert_queries_f:
            values = state.values[start:end]
            self._cur.execute(query, values)
        # self._profiling["insert_value"] += time.perf_counter() - tt

    def get_applicable_actions(self, state: SGState) -> list[SGAction]:
        # TODO inserting state is a bottleneck and redundant
        t = time.perf_counter()
        # tt = time.perf_counter()
        self._clear_state()
        # self._profiling["clear"] += time.perf_counter() - tt
        self._insert_state(state)
        self._t_aag_processing += time.perf_counter() - t

        # generate applicable actions
        t = time.perf_counter()
        actions = []
        for schema, query in enumerate(self._schema_queries):
            # print(self._task.i_to_schema[schema], "\n", query, "\n")
            for row in self._cur.execute(query).fetchall():
                actions.append((schema, row))
        self._t_aag_execution += time.perf_counter() - t
        # self._profiling["query"] = self._t_aag_execution
        return actions

    """Verbosity methods"""

    def _get_current_db_state(self, pddl_format: bool = True, delimiter: str = ",") -> str:
        state = set()
        for p in self._predicates:
            for objs in self._exec_dev(f"SELECT {self._sql_terms_str(p, constants=False)} FROM {to_table_name(p)}"):
                pred_name = p.name
                if p.arity == 0:
                    state.add(pred_name)
                    continue
                if pddl_format:
                    fact = "(" + " ".join([pred_name] + [self._i_to_obj[o] for o in objs]) + ")"
                else:
                    fact = f"{pred_name}({','.join(self._i_to_obj[o] for o in objs)})"
                state.add(fact)

        for f in self._functions:
            for objs in self._exec_dev(f"SELECT {self._sql_terms_str(f, constants=False)} FROM {to_table_name(f)}"):
                if pddl_format:
                    fact = "(" + " ".join([f.name] + [self._i_to_obj[o] for o in objs[:-1]]) + ")"
                    fact = f"(= {fact} {objs[-1]})"
                else:
                    fact = f"{f.name}({','.join(self._i_to_obj[o] for o in objs[:-1])})"
                    fact = f"{fact} == {objs[-1]}"
                state.add(fact)

        for t in self._domain.types:
            table_name = to_table_name(t, desc="type")
            for obj in self._exec_dev(f"SELECT {_PARAM} FROM {table_name}"):
                obj_name = self._i_to_obj[obj[0]]
                fact = f"{obj_name} - {t}"
                state.add(fact)

        return delimiter.join(sorted(state))

    def dump_db(self) -> None:
        print(self._get_current_db_state(pddl_format=True, delimiter="\n"))

    def dump_profiling(self) -> None:
        total_time = sum(self._profiling.values())
        if total_time == 0:
            return
        mat = []
        for k, v in self._profiling.items():
            mat.append([k, f"{v:.3f}", f"{v/total_time:.2%}"])
        mat.append(["Total", f"{total_time:.3f}", "100.00%"])
        print(mat_to_str(mat))
        print(mat_to_str(mat))
