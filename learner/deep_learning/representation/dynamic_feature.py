""" 
TODO: 3 different types of DE
1. singleton
2. batch may be from different problems
3. batch but all from same problem
"""

from argparse import Namespace
from dataclasses import dataclass
from types import FunctionType

import numpy as np
from torch import Tensor


class DynamicEvaluator:
    def __init__(self, G, opts: Namespace) -> None:
        self.expr_and_indices = G.get_expression_and_indices()
        self.dynamic_features = opts.dynamic_features

    def transform(self, x: Tensor, offset: int = 0) -> None:
        """in-place operation"""
        if not self.dynamic_features:
            return

        for expr_and_indices in self.expr_and_indices:
            index = expr_and_indices.index + offset
            neighbour_indices = expr_and_indices.neighbour_indices + offset
            f = expr_and_indices.function

            inputs = x[neighbour_indices][:, 0]
            output = f(*inputs)

            x[index, 0] = output
