"""
Big wrapper for the NN model and the representation. It is also the object that
is called from NFD (c++) through pybind.
"""

import logging
import os
import time
import traceback
from argparse import Namespace
from typing import Dict, List, Set, Tuple

import numpy as np
import torch
from torch_geometric.data import Data
from torch_geometric.loader import DataLoader

from learner.deep_learning.nn.base import Gnn
from learner.deep_learning.nn.gnn_input import GnnInput
from learner.deep_learning.nn.train_info import GnnTrainInfo
from learner.deep_learning.representation.feature_generator import FluentFeature
from learner.deep_learning.representation.gnn_representation import GnnRepresentation
from learner.model import Model
from learner.problem.numeric_state import NumericState
from learner.problem.util import var_to_predicate

logging.basicConfig(
    level=logging.INFO,
    # level=logging.DEBUG,
    format="%(levelname)s [%(filename)s:%(lineno)s] %(message)s",
)


class DeepLearningModel(Model):
    def __init__(self) -> None:
        super().__init__()
        self.representation: GnnRepresentation = None
        self.nn: Gnn = None
        self.train_info: GnnTrainInfo = None

        ## timers during search
        self._graph_time = 0.0
        self._dataloader_time = 0.0
        self._gnn_time = 0.0

    # Python cannot overload methods e.g. for initialising a model with
    # arguments or initialising by loading from file both through __init__
    def set_objects(
        self, opts: Namespace, representation: GnnRepresentation, model: Gnn
    ) -> None:
        self.opts = opts
        self.representation = representation
        self.nn = model

    def train(self) -> None:
        self.nn.train()

    def eval(self) -> None:
        self.nn.eval()

    def forward(self, data: Data) -> torch.Tensor:
        return self.nn.forward(data)

    def load(self, load_file: str, device: int = None) -> None:
        self._set_device(device)
        super().load(load_file)
        self.nn.to(self.device)
        self.eval()
        logging.info(f"Model loaded successfully from {load_file}")

    @property
    def multi_heuristics(self) -> bool:
        return False

    @property
    def pref_schema(self) -> bool:
        return False  # TODO

    def dump(self) -> None:
        self.nn.dump()
        self.representation.dump()
        if self.train_info is not None:
            self.train_info.dump()
        print(f"Device: {self.device}")
        if self.device.type == "cuda":
            device_name = torch.cuda.get_device_name()
            device_number = torch.cuda.current_device()
            print(f"Device name: {device_name}")
            print(f"Device number: {device_number}")

    """ All methods below are called only in cpp """

    def set_domain_problem(
        self, domain_pddl: str, problem_pddl: str, nfd_vars_string: str
    ) -> None:
        super().set_domain_problem(domain_pddl, problem_pddl, nfd_vars_string)
        self.representation.set_problem(self.problem)
        self.d_eval = self.representation.get_dynamic_evaluator()
        logging.info(f"Domain and problem set successfully!")

    def get_graph_x(self) -> List[List[float]]:
        x = self.representation.graph.x.tolist()
        return x

    def get_n_edge_labels(self) -> int:
        return len(self.representation.graph.edge_indices)

    def get_graph_edge_indices_0(self, idx: int) -> List[int]:
        edge_indices = self.representation.graph.edge_indices
        ret = edge_indices[idx][0].tolist()
        return ret

    def get_graph_edge_indices_1(self, idx: int) -> List[int]:
        edge_indices = self.representation.graph.edge_indices
        ret = edge_indices[idx][1].tolist()
        return ret

    def get_bool_label_offset(self) -> int:
        return self.representation.label_generator.bool_label_offset

    def get_num_goal_offset(self) -> int:
        return self.representation.feature_generator.num_goal_offset

    def get_fluent_offset(self) -> int:
        return self.representation.feature_generator.fluent_offset

    def get_name_to_idx(self) -> Dict[str, int]:
        ret = {}
        for k, v in self.representation.graph.node_to_idx.items():
            # print(k, v, flush=True)
            ret[k] = v
            if "(" not in k:  # unified_planning being annoying
                k = f"{k}()"
                # print(k, v, flush=True)
                ret[k] = v
        return ret

    def get_bool_goals(self) -> Set[str]:
        return self.representation.graph.bool_goals

    def get_fact_pred_to_idx(self) -> Dict[str, int]:
        return self.representation.feature_generator._pred_to_idx

    def get_fluent_feat_idx(self, fluent_name: str) -> int:
        try:
            predicate = var_to_predicate(fluent_name)
            idx = self.representation.feature_generator.fluent_node_feat_idx(
                predicate, FluentFeature.VALUE
            )
        except:
            traceback.print_exc()
            print("", flush=True)
            exit(-1)
        return idx

    def get_num_goal_updates(
        self, true_bools: List[str], num_vals: List[float]
    ) -> List[Tuple[int, int, float]]:
        nfd_state = NumericState(true_bools, dict(zip(self._nfd_fluents, num_vals)))
        ret = self.representation.graph.nfd_state_to_num_goal_evaluations(nfd_state)
        return ret

    """ Actual GNN code """

    def evaluate(self, true_bools: List[str], num_vals: List[float]) -> float:
        try:
            nfd_state = NumericState(true_bools, dict(zip(self._nfd_fluents, num_vals)))

            gnn_input: GnnInput = self.representation.nfd_state_to_gnn_input(nfd_state)
            gnn_input.to(self.device)

            with torch.no_grad():
                h = self.nn.evaluate(
                    x=gnn_input.x,
                    edge_indices=gnn_input.edge_indices,
                    d_eval=self.d_eval,
                    batch=None,
                )

            if self.opts.round:
                h = torch.round(h)  # assumed unit cost in plans

            # print(num_vals)
            # nfd_state.dump()
            # gnn_input.dump(verbosity=1)
            # logging.debug(h)
            # breakpoint()
        except:
            traceback.print_exc()
            print("", flush=True)
            exit(-1)

        return h

    ## We have evaluate_batch and evaluate_batch_py versions with the former being
    ## more optimised by not needing to construct the graph in python which turns out
    ## to be a substantial bottleneck (>50%)

    def evaluate_batch_py(
        self, list_true_bools: List[List[bool]], list_num_vals: List[List[float]]
    ) -> List[float]:

        nfd_states = [
            NumericState(true_bools, dict(zip(self._nfd_fluents, num_vals)))
            for true_bools, num_vals in zip(list_true_bools, list_num_vals)
        ]

        dataset = []
        for nfd_state in nfd_states:
            t = time.time()
            gnn_input = self.representation.nfd_state_to_gnn_input(nfd_state)
            self._graph_time += time.time() - t

            # gnn_input.dump(1)
            # exit(-1)

            t = time.time()
            data = Data(
                x=gnn_input.x,
                edge_index=gnn_input.edge_indices,
                d_eval=self.d_eval,
            )
            dataset.append(data)
            self._dataloader_time += time.time() - t

        return self._evalute_batch_from_dataset(dataset)

    def evaluate_batch(
        self,
        x_list: List[List[List[float]]],
        edge_indices_list: List[List[List[List[int]]]],
    ) -> List[float]:
        dataset = []
        for x, edge_indices in zip(x_list, edge_indices_list):
            t = time.time()
            data = Data(
                ## TODO: converting to tensor is a significant bottleneck
                x=torch.tensor(x),
                edge_index=[torch.tensor(e).long() for e in edge_indices],
                d_eval=self.d_eval,
            )
            dataset.append(data)
            self._dataloader_time += time.time() - t

        return self._evalute_batch_from_dataset(dataset)

    def _evalute_batch_from_dataset(self, dataset: List[Data]) -> List[float]:
        # TODO read max batch size from data
        t = time.time()
        loader = DataLoader(dataset=dataset, batch_size=min(len(dataset), 64))
        self._dataloader_time += time.time() - t

        t = time.time()
        hs_all = []
        with torch.no_grad():
            for dataset in loader:
                dataset = dataset.to(self.device)
                hs = self.nn.evaluate(
                    x=dataset.x,
                    edge_indices=dataset.edge_index,
                    d_eval=dataset.d_eval,
                    batch=dataset.batch,
                )
                hs = hs.detach().cpu().numpy()
                hs_all.append(hs)
        hs_all = np.concatenate(hs_all)
        if self.opts.round:
            hs_all = np.rint(hs_all)
        hs_all = hs_all.tolist()
        self._gnn_time += time.time() - t

        return hs_all

    ### get timer information
    def get_graph_time(self) -> float:
        return self._graph_time

    def get_dataloader_time(self) -> float:
        return self._dataloader_time

    def get_gnn_time(self) -> float:
        return self._gnn_time
