import argparse

import torch
from typing_extensions import override

from planning.policy.policy import PolicyExecutor
from wlplan.feature_generator import load_feature_generator
from wlplan.graph_generator import Graph


class WlfPolicyExecutor(PolicyExecutor):
    def __init__(
        self,
        domain_path: str,
        problem_path: str,
        params_path: str,
        train_opts: argparse.Namespace,
        debug: bool = False,
        bound: int = -1,
    ):
        super().__init__(
            domain_path=domain_path,
            problem_path=problem_path,
            train_opts=train_opts,
            debug=debug,
            bound=bound,
        )

        # Set seeds
        self._fg = load_feature_generator(params_path)

    @override
    def _predict_graph(self, graph: Graph) -> float:
        pred = self._fg.predict(graph)
        return pred
