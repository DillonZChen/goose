import itertools
from typing import Optional

from pddl.action import Action
from pddl.core import Domain, Problem
from pddl.custom_types import name as name_type
from pddl.logic import Predicate, Variable
from pddl.logic.base import And, BinaryOp, Formula, UnaryOp
from pddl.logic.effects import When
from pddl.logic.functions import BinaryFunction, NumericFunction, NumericValue
from pddl.logic.predicates import EqualTo
from pddl.logic.terms import Constant, Term

TypeTag = Optional[name_type]


def _ground_term(term: Term, mapping: dict[Variable, Constant]) -> Constant:
    key = str(term)
    if isinstance(term, Constant):
        return term
    elif isinstance(term, Variable):
        return mapping[key]
    else:
        raise TypeError(f"{term}: unknown term type: {type(term)}")


def _ground_formula(op: Formula, mapping: dict[Variable, Constant]) -> Formula:
    optype = type(op)
    if optype == Predicate:
        return Predicate(op.name, *[_ground_term(t, mapping) for t in op.terms])
    elif optype == EqualTo:
        return EqualTo(_ground_term(op.left, mapping), _ground_term(op.right, mapping))
    elif issubclass(optype, UnaryOp):
        return optype(_ground_formula(op.argument, mapping))
    elif issubclass(optype, BinaryOp):
        return optype(*[_ground_formula(t, mapping) for t in op.operands])
    elif optype == And:
        return optype(*[_ground_formula(t, mapping) for t in op.operands])
    elif optype == When:
        return optype(_ground_formula(op.condition, mapping), _ground_formula(op.effect, mapping))
    elif issubclass(optype, BinaryFunction):
        return optype(*[_ground_formula(t, mapping) for t in op.operands])
    elif optype == NumericValue:
        return op
    elif issubclass(optype, NumericFunction):
        return optype(op.name, *[_ground_term(t, mapping) for t in op.terms])
    else:
        raise TypeError(f"{op}: unknown operator type: {optype}")


def _is_subtype(subtype: TypeTag, supertype: TypeTag, type_dict: dict[TypeTag, TypeTag]) -> bool:
    if subtype == supertype:
        return True
    if subtype in type_dict:
        return _is_subtype(type_dict[subtype], supertype, type_dict)
    return False


def _check_types(constant: Constant, variable: Variable, type_dict: dict[TypeTag, TypeTag]) -> bool:
    if not type_dict:
        # There are no types, so we assume everything is of the same type.
        return True
    return any(_is_subtype(constant.type_tag, v_type, type_dict) for v_type in variable.type_tags)


def ground_action(domain: Domain, action, grounding: tuple[Constant]) -> Optional[Action]:
    if not isinstance(action, Action):
        action = [a for a in domain.actions if a.name == action][0]
    mapping = {str(k): v for k, v in zip(action.parameters, grounding)}
    # assert all(_check_types(c, v, domain.types) for v, c in mapping.items())
    # if not all(_check_types(c, v, domain.types) for v, c in mapping.items()):
    #     return None
    ground_precondition = _ground_formula(action.precondition, mapping)
    ground_effect = _ground_formula(action.effect, mapping)
    return Action(
        action.name,
        parameters=grounding,
        precondition=ground_precondition,
        effect=ground_effect,
    )


def ground(domain: Domain, problem: Problem) -> list[Action]:
    constants = domain.constants | problem.objects
    operators = []
    for action in domain.actions:
        for grounding in itertools.product(constants, repeat=len(action.parameters)):
            op = ground_action(domain, action, grounding)
            if op:
                operators.append(op)
    return sorted(operators, key=lambda a: (a.name, a.parameters))


def ground_domain_predicates(domain: Domain, problem: Problem) -> set[Predicate]:
    constants = domain.constants | problem.objects
    ground_predicates = set()
    for predicate in domain.predicates:
        for grounding in itertools.product(constants, repeat=predicate.arity):
            mapping = dict(zip(predicate.terms, grounding))
            if not all(_check_types(c, v, domain.types) for v, c in mapping.items()):
                continue
            ground_predicate = _ground_formula(predicate, mapping)
            ground_predicates.add(ground_predicate)
    return ground_predicates
