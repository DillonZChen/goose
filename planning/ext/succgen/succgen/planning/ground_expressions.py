from _succgen.planning import (
    GroundBooleanExpression,
    GroundDivide,
    GroundEqualTo,
    GroundFunction,
    GroundGreaterThan,
    GroundGreaterThanEqualTo,
    GroundLessThan,
    GroundLessThanEqualTo,
    GroundMinus,
    GroundNot,
    GroundOr,
    GroundPlus,
    GroundTimes,
    GroundValue,
)
from pddl.logic.base import BinaryOp, Not, Or
from pddl.logic.functions import (
    BinaryFunction,
    Divide,
    EqualTo,
    FunctionExpression,
    GreaterEqualThan,
    GreaterThan,
    LesserEqualThan,
    LesserThan,
    Minus,
    NumericFunction,
    NumericValue,
    Plus,
    Times,
)
from succgen.planning import PDDLState
from succgen.planning.strings import to_value


def to_ground_expr(
    expr: FunctionExpression, statics: PDDLState, num_fluent_opt_to_i, func_to_i, obj_to_i
) -> GroundBooleanExpression:
    if isinstance(expr, Not):
        return GroundNot(to_ground_expr(expr.argument, statics, num_fluent_opt_to_i, func_to_i, obj_to_i))
    elif isinstance(expr, Or):
        return GroundOr(
            [to_ground_expr(arg, statics, num_fluent_opt_to_i, func_to_i, obj_to_i) for arg in expr.operands]
        )
    elif isinstance(expr, NumericValue):
        return GroundValue(to_value(expr))
    elif isinstance(expr, NumericFunction):
        if expr in statics:
            return GroundValue(statics[expr])
        table = func_to_i[expr.name]
        row = tuple(obj_to_i[obj.name] for obj in expr.terms)
        i = num_fluent_opt_to_i[(table, row)]
        return GroundFunction(i)
    else:
        assert isinstance(expr, (BinaryOp, BinaryFunction)), expr
        lhs = expr.operands[0]
        rhs = expr.operands[1]
        if isinstance(expr, Plus):
            Cls = GroundPlus
        elif isinstance(expr, Minus):
            Cls = GroundMinus
        elif isinstance(expr, Times):
            Cls = GroundTimes
        elif isinstance(expr, Divide):
            Cls = GroundDivide
        elif isinstance(expr, EqualTo):
            Cls = GroundEqualTo
        elif isinstance(expr, LesserThan):
            Cls = GroundLessThan
        elif isinstance(expr, LesserEqualThan):
            Cls = GroundLessThanEqualTo
        elif isinstance(expr, GreaterThan):
            Cls = GroundGreaterThan
        elif isinstance(expr, GreaterEqualThan):
            Cls = GroundGreaterThanEqualTo
        else:
            raise NotImplementedError(f"Unknown binary operator {expr} of type {type(expr)}")
        return Cls(
            to_ground_expr(lhs, statics, num_fluent_opt_to_i, func_to_i, obj_to_i),
            to_ground_expr(rhs, statics, num_fluent_opt_to_i, func_to_i, obj_to_i),
        )
