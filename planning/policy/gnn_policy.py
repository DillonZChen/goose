import argparse

import torch
from typing_extensions import override

from learning.predictor.neural_network.gnn import RGNN
from learning.predictor.neural_network.serialise import load_gnn_weights
from learning.pyg import wlplan_graph_to_pyg
from planning.policy.policy import PolicyExecutor
from wlplan.graph_generator import Graph


class GnnPolicyExecutor(PolicyExecutor):
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

        self._device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

        model = RGNN.init_from_opts(opts=train_opts)
        model.load_state_dict(state_dict=load_gnn_weights(params_path))
        model = model.to(self._device)

        self._gnn = model

    @override
    def _predict_graph(self, graph: Graph) -> float:
        pyg = wlplan_graph_to_pyg(self._graph_generator, graph).to(self._device)
        pred = self._gnn.forward(x=pyg.x, edge_indices_list=pyg.edge_index)
        pred = pred.squeeze().item()
        return pred
