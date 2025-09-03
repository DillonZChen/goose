from _succgen.planning import (
    FluentValueMap,
    LiftedDivide,
    LiftedExpression,
    LiftedFunction,
    LiftedMinus,
    LiftedPlus,
    LiftedTimes,
    LiftedValue,
    StaticLiftedFunction,
)
from pddl.logic.base import BinaryOp
from pddl.logic.functions import (
    BinaryFunction,
    Divide,
    FunctionExpression,
    Minus,
    NumericFunction,
    NumericValue,
    Plus,
    Times,
)
from succgen.planning.strings import to_value
from succgen.planning.task import SGTask


def to_lifted_expr(expr: FunctionExpression, param_to_index: dict[str, int], task: SGTask) -> LiftedExpression:
    if isinstance(expr, NumericValue):
        return LiftedValue(to_value(expr))
    elif isinstance(expr, NumericFunction):
        table = task.func_to_i[expr.name]
        row = [param_to_index[obj.name] for obj in expr.terms]

        if table in task.static_functions:
            values = []
            for key, value in task.statics.items():
                if key.name == expr.name:
                    constants = tuple([task.obj_to_i[obj.name] for obj in key.terms])
                    values.append(((table, constants), value))
            return StaticLiftedFunction(fluent=(table, row), values=FluentValueMap(values))
        else:
            return LiftedFunction(fluent=(table, row))
    elif isinstance(expr, (BinaryOp, BinaryFunction)):
        lhs = expr.operands[0]
        rhs = expr.operands[1]
        if isinstance(expr, Plus):
            Cls = LiftedPlus
        elif isinstance(expr, Minus):
            Cls = LiftedMinus
        elif isinstance(expr, Times):
            Cls = LiftedTimes
        elif isinstance(expr, Divide):
            Cls = LiftedDivide
        else:
            raise NotImplementedError(f"Unknown binary operator {expr} of type {type(expr)}")
        return Cls(
            to_lifted_expr(expr=lhs, param_to_index=param_to_index, task=task),
            to_lifted_expr(expr=rhs, param_to_index=param_to_index, task=task),
        )
    else:
        raise NotImplementedError(f"Unknown expression {expr} of type {type(expr)}")
