from typing import Union

import lark
from pddl.action import Action
from pddl.core import Domain, Problem
from pddl.logic import Predicate
from pddl.logic.base import Not
from pddl.logic.functions import (
    EqualTo,
    GreaterEqualThan,
    GreaterThan,
    LesserEqualThan,
    LesserThan,
    NumericFunction,
    NumericValue,
)
from succgen.planning import PDDLState, get_domain, get_problem
from succgen.planning.ground import ground_action


ClassicCondition = Predicate | Not
NumericCondition = EqualTo | GreaterEqualThan | GreaterThan | LesserEqualThan | LesserThan


def get_numeric_condition_symbol(condition: NumericCondition) -> str:
    if isinstance(condition, GreaterEqualThan):
        return ">="
    elif isinstance(condition, EqualTo):
        return "=="
    elif isinstance(condition, GreaterThan):
        return ">"
    elif isinstance(condition, LesserEqualThan):
        return "<="
    elif isinstance(condition, LesserThan):
        return "<"
    else:
        raise NotImplementedError(f"Condition of type {type(condition)} is not supported yet: {condition}")


def condition_to_string(p: str | ClassicCondition | NumericCondition) -> str:
    if isinstance(p, Predicate | NumericFunction):
        return f'{p.name}({",".join([str(p) for p in p.terms])})'
    elif isinstance(p, str):
        return p.replace(" ", "")
    elif isinstance(p, float | int | lark.lexer.Token | NumericValue):
        return to_value(p)
    elif isinstance(p, NumericCondition):
        lhs = condition_to_string(p.operands[0])
        rhs = condition_to_string(p.operands[1])
        symbol = get_numeric_condition_symbol(p)
        return f"{symbol}({lhs},{rhs})"
    else:
        raise ValueError("Unknown state type: {}".format(type(p)))


def state_to_string(state: PDDLState | list[str], delimiter: str = ",", sort: bool = True) -> str:
    state_str = []
    for p in state:
        state_str.append(condition_to_string(p))
    if sort:
        state_str = sorted(state_str)
    return delimiter.join(state_str)


def action_to_string(action: Action, mapping: list[str] | None = None, plan_style: bool = False) -> str:
    if mapping is None:
        mapping = [str(p) for p in action.parameters]
    if plan_style:
        return f"({' '.join([action.name] + mapping)})"
    else:
        return f"{action.name}({','.join(mapping)})"


def strings_to_actions(domain: str | Domain, problem: str | Problem, actions: list[str]) -> list[Action]:
    domain = get_domain(domain)
    problem = get_problem(problem)
    converter = StringActionConverter(domain, problem)
    return [converter.string_to_action(action) for action in actions]


class StringActionConverter:
    def __init__(self, domain: Domain, problem: Problem):
        self.domain = domain
        self.name_to_action = {action.name.lower(): action for action in domain.actions}
        self.name_to_object = {obj.name: obj for obj in problem.objects | domain.constants}

    def string_to_action(self, action_str: str) -> Action:
        assert action_str[0] == "(", action_str
        assert action_str[-1] == ")", action_str
        action_str = action_str[1:-1]
        action_name, *params = action_str.split()
        objects = [self.name_to_object[param] for param in params]
        action = ground_action(self.domain, self.name_to_action[action_name.lower()], tuple(objects))
        return action


def to_value(token: Union[NumericValue, float, int]) -> Union[float, int]:
    if isinstance(token, int | float):
        return token
    token = token.value
    if isinstance(token, int | float):
        return token
    if "." in token:
        return float(token)
    else:
        return int(token)
