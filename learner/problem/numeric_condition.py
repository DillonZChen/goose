from abc import ABC
from types import FunctionType
from typing import List, Set

from learner.problem.numeric_state import NumericState
from learner.problem.util import var_to_objects, var_to_predicate


class NumericCondition(ABC):
    def __init__(self, expr: str, val: float, comparator: str, vars: Set[str]) -> None:
        self._variables: List[str] = None
        self._expr: str = None

        assert comparator in {"<=", ">=", "="}
        if comparator == "=":
            comparator = "=="
        self.comparator = comparator
        self._variables: List[str] = []
        ret_expr = expr
        for var in vars:
            pred = var_to_predicate(var)
            objects = var_to_objects(var)
            if len(objects) == 0:
                pddl_var = f"({pred})"
            else:
                pddl_var = f"({pred} {' '.join(objects)})"

            if pddl_var in expr:
                self._variables.append(var)
                expr = expr.replace(pddl_var, "")
                ret_expr = ret_expr.replace(pddl_var, var)
        assert "," not in expr, expr

        self._expr: str = ret_expr
        if val != 0:
            self._expr = f"{ret_expr} - {val}"

    def get_variables(self) -> List[str]:
        return self._variables

    def get_error_function(self) -> FunctionType:
        """create Python function object dynamically from strings"""
        # https://stackoverflow.com/a/11291851/13531424
        # we want it to be differentiable for backprop
        var_map = {v: f"_x{i}_" for i, v in enumerate(self._variables)}
        expr = self._expr
        for var, new_var in var_map.items():
            expr = expr.replace(var, new_var)

        if self.comparator in {"<=", "<"}:
            func = f"def _func({', '.join(var_map.values())}): return max({expr}, 0)"
        elif self.comparator in {">=", ">"}:
            func = f"def _func({', '.join(var_map.values())}): return -max({expr}, 0)"
        else:
            func = f"def _func({', '.join(var_map.values())}): return abs({expr})"
        exec(func)
        f = locals()["_func"]
        return f

    def nfd_evaluate_expr(self, state: NumericState) -> float:
        """Evaluate the expression on the input state"""
        # multiple replace might seem that it would be slow but according to
        # https://stackoverflow.com/q/3411006/13531424 it's not too bad
        expr = self._expr
        for var, val in state.fluent_values.items():
            expr = expr.replace(var, str(val))
        expr = eval(expr)  # dangerous!!!
        return expr

    def error(self, value: float) -> float:
        if self.comparator in {"<=", "<"}:
            error = max(value, 0)  # expr if expr > 0 else 0
        elif self.comparator in {">=", ">"}:
            error = -max(value, 0)
        else:
            error = abs(value)
        return error

    def achieved(self, value: float) -> bool:
        result = eval(f"{value} {self.comparator} 0")
        return result

    def __repr__(self) -> str:
        return f"{self._expr} {self.comparator} 0"
