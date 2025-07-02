import logging
from typing import Any, Mapping, Optional, Union

import pddl
import pddl.requirements
from pddl.core import Domain, Problem
from pddl.logic import Predicate
from pddl.logic.base import And, Formula, Not
from pddl.logic.functions import (
    Decrease,
    EqualTo,
    GreaterEqualThan,
    GreaterThan,
    Increase,
    LesserEqualThan,
    LesserThan,
    NumericFunction,
    NumericValue,
)
from succgen.sgutil.logging import mat_to_str


__all__ = ["get_plan", "Plan", "PDDLState", "Literal"]

Plan = list[str]
Literal = Predicate | Not | EqualTo | GreaterEqualThan | GreaterThan | LesserEqualThan | LesserThan
PDDLState = frozenset[Union[Predicate, EqualTo]]


def get_domain(domain: Union[Domain, str]) -> Domain:
    if isinstance(domain, str):
        domain = pddl.parse_domain(domain)
    assert isinstance(domain, Domain)
    return domain


def get_problem(problem: Union[Problem, str]) -> Problem:
    if isinstance(problem, str):
        problem = pddl.parse_problem(problem)
    assert isinstance(problem, Problem)
    return problem


def get_pred_index_mappings(domain: Domain) -> tuple[dict[str, int], list[str]]:
    pred_to_i = {}
    i_to_pred = []
    for i, p in enumerate(sorted(domain.predicates, key=lambda p: p.name)):
        pred_to_i[p.name] = i
        i_to_pred.append(p.name)
    return pred_to_i, i_to_pred


def get_func_index_mappings(domain: Domain) -> tuple[dict[str, int], list[str]]:
    func_to_i = {}
    i_to_func = []
    for i, f in enumerate(sorted(domain.functions, key=lambda f: f.name)):
        func_to_i[f.name] = i
        i_to_func.append(f.name)
    return func_to_i, i_to_func


def get_obj_index_mappings(domain: Domain, problem: Problem) -> tuple[dict[str, int], list[str]]:
    obj_to_i = {}
    i_to_obj = []
    for o in sorted(domain.constants, key=lambda o: o.name):
        obj_to_i[o.name] = len(obj_to_i)
        i_to_obj.append(o.name)
    for o in sorted(problem.objects, key=lambda o: o.name):
        if o.name not in obj_to_i:
            obj_to_i[o.name] = len(obj_to_i)
            i_to_obj.append(o.name)
    for i, obj in enumerate(i_to_obj):
        assert obj_to_i[obj] == i
    return obj_to_i, i_to_obj


def get_schema_index_mappings(domain: Domain) -> tuple[dict[str, int], list[str]]:
    schema_to_i = {}
    i_to_schema = []
    for i, s in enumerate(sorted(domain.actions, key=lambda s: s.name)):
        schema_to_i[s.name] = i
        i_to_schema.append(s.name)
    return schema_to_i, i_to_schema


def is_numeric_domain(domain: Domain) -> bool:
    return len(domain.functions) > 0 or pddl.requirements.Requirements.NUMERIC_FLUENTS in domain.requirements


def new_problem(domain: Domain, problem: Problem, state: PDDLState, goal: Optional[Formula] = None) -> Problem:
    if goal is None:
        goal = problem.goal
    return Problem(
        name=f"p{hash(state)}{hash(goal)}",
        domain=domain,
        objects=problem.objects,
        init=state,
        goal=goal,
    )


def get_condition_list(formula: Formula) -> list[Formula]:
    if isinstance(formula, And):
        return list(formula.operands)
    elif isinstance(formula, Literal | Increase | Decrease):
        return [formula]
    else:
        raise NotImplementedError(f"Not supported formula type: {type(formula)}")


def get_literals_list(formula: Formula) -> list[Literal]:
    if isinstance(formula, And):
        return list(formula.operands)
    elif isinstance(formula, Not):
        return [formula.argument]
    elif isinstance(formula, Literal):
        return [formula]
    else:
        raise NotImplementedError(f"Not supported formula type: {type(formula)}")


def get_domain_statistics(domain: Domain) -> Mapping:
    ret = {
        "types": len(domain.types),
        "constants": len(domain.constants),
        "predicates": len(domain.predicates),
        "functions": len(domain.functions),
        "axioms": len(domain.derived_predicates),
        "actions": len(domain.actions),
    }
    # for action in domain.actions:
    #     ret[f"{action.name} pre"] = len(get_literals_list(action.precondition))
    #     ret[f"{action.name} eff"] = len(get_literals_list(action.effect))
    return ret


def get_problem_statistics(problem: Problem) -> Mapping:
    return {
        "objects": len(problem.objects),
        "init": len(problem.init),
        "goal": len(get_literals_list(problem.goal)),
    }


def dump_statistics(domain: Domain, problem: Problem, description_prefix: str) -> None:
    domain_stats = get_domain_statistics(domain)
    problem_stats = get_problem_statistics(problem)
    mat_str = mat_to_str(
        [("Domain", "")]
        + [(k, v) for k, v in domain_stats.items()]
        + [("Problem", "")]
        + [(k, v) for k, v in problem_stats.items()]
    )
    logging.info(f"{description_prefix}{mat_str}")
