from dataclasses import dataclass
from types import FunctionType
from typing import Optional, SupportsFloat, Union

from _succgen.planning import Effects

from succgen.planning.lifted_expressions import LiftedExpression

__all__ = ["NumericEffect", "NumericEffects", "Effects"]


@dataclass
class NumericEffect:
    table: int
    row: tuple[int]  # int refers to the index of the parameters to be instantiated
    effect: FunctionType
    expr: Union[SupportsFloat, LiftedExpression]


NumericEffects = list[NumericEffect]
