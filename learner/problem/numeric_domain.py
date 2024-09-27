from typing import List

from unified_planning.io.pddl_reader import PDDLReader
from unified_planning.model.action import Action
from unified_planning.model.problem import Problem


class NumericDomain:
    def __init__(self, domain_pddl) -> None:
        self.domain_pddl = domain_pddl

        # to be filled out below
        self.max_func_arity = 0
        self.max_pred_arity = 0
        self.max_action_arity = 0

        reader = PDDLReader()
        problem: Problem = reader.parse_problem(domain_pddl)
        self.name = problem.name
        ## predicates and functions
        self.symbols: List[str] = [p.name for p in problem.fluents]
        self.predicates: List[str] = []
        self.functions: List[str] = []
        self.schemata: List[Action] = problem.actions
        self.object_types: List = [t.name for t in problem.user_types]

        self._predicate_arity = {}

        for symbol in problem.fluents:
            var_type = repr(symbol.type)
            arity = symbol.arity
            self._predicate_arity[symbol.name] = arity
            if var_type == "real":
                self.max_func_arity = max(self.max_func_arity, arity)
                self.functions.append(symbol.name)
            elif var_type == "bool":
                self.max_pred_arity = max(self.max_pred_arity, arity)
                self.predicates.append(symbol.name)
            else:
                raise ValueError(f"Unknown var of type {var_type}")

        for schema in self.schemata:
            self.max_action_arity = max(self.max_action_arity, len(schema.parameters))

        ## statics
        symbols = set(self.symbols)
        to_keep = set()
        for schema in self.schemata:

            for effect in schema._effects:
                effect = str(effect)
                if "(" in effect:
                    symbol = effect.split("(")[0]
                else:
                    toks = effect.split(" ")
                    assert toks[1] in {":=", "-=", "+="}, effect
                    symbol = toks[0]
                assert symbol in symbols, symbol
                to_keep.add(symbol)

        self.static_symbols = symbols - to_keep
        self.functions = sorted(list(set(self.functions) - self.static_symbols))
        self.predicates = sorted(list(set(self.predicates) - self.static_symbols))

    def arity(self, predicate: str) -> int:
        return self._predicate_arity[predicate]
