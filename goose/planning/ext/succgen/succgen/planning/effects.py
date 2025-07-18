from dataclasses import dataclass
from types import FunctionType
from typing import SupportsFloat, Union

from _succgen.planning import AssignEffect, DecreaseEffect, Effects, IncreaseEffect
from succgen.planning.lifted_expressions import LiftedExpression


__all__ = ["NumericEffect", "NumericEffects", "Effects", "AssignEffect", "IncreaseEffect", "DecreaseEffect"]


@dataclass
class NumericEffect:
    table: int
    row: tuple[int]  # int refers to the index of the parameters to be instantiated
    effect: FunctionType
    expr: Union[SupportsFloat, LiftedExpression]


NumericEffects = list[NumericEffect]
