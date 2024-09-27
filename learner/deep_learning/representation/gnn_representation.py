from argparse import Namespace
from typing import List, Optional

import torch
from torch_geometric.data import Data
from tqdm import tqdm

from learner.dataset.preferred_schema_dataset import \
    optimal_actions_to_multilabel_schema
from learner.dataset.ranking_data import RankingData
from learner.dataset.raw_dataset import RawDataset
from learner.deep_learning.nn.gnn_input import GnnInput
from learner.deep_learning.representation.dynamic_feature import DynamicEvaluator
from learner.deep_learning.representation.feature_generator import FeatureGenerator
from learner.deep_learning.representation.graph import Graph
from learner.deep_learning.representation.label_generator import LabelGenerator
from learner.problem.numeric_problem import NumericProblem
from learner.problem.numeric_state import NumericState
from learner.representation import Representation
from util.statistics import dump_several_stats, print_mat


class GnnRepresentation(Representation):
    def __init__(self, opts: Namespace) -> None:
        super().__init__(opts)
        self.graph = None

        # representation parameters
        self.numeric_agnostic = opts.numeric_agnostic
        self.dynamic_features = opts.dynamic_features

        # features
        self.feature_generator = FeatureGenerator(self.domain_pddl)
        self.label_generator = LabelGenerator(self.domain_pddl)
        self.n_node_features = self.feature_generator.n_features
        self.n_edge_labels = self.label_generator.n_labels

    def transform_heuristic_dataset(self, dataset: RawDataset, **kwargs) -> List[Data]:
        ret = []
        for problem, state_data_list in tqdm(dataset.items()):
            self.set_problem(problem)
            dynamic_evaluator = self.get_dynamic_evaluator()
            for state_data in state_data_list:
                if not state_data.description == "opt":
                    continue
                gnn_input = self.nfd_state_to_gnn_input(state_data.state)
                pyg_data = Data(
                    x=gnn_input.x,
                    edge_index=gnn_input.edge_indices,
                    y=state_data.heuristic,
                    d_eval=dynamic_evaluator,
                )
                ret.append(pyg_data)

        ## log info
        dump_several_stats(([data.y for data in ret], "targets"))

        return ret

    def transform_ranking_dataset(self, ranking_data: List[RankingData], **kwargs):
        ret = []
        for r_data in ranking_data:
            self.set_problem(r_data.problem)
            dynamic_evaluator = self.get_dynamic_evaluator()
            graphs = []

            for state in r_data.states:
                gnn_input = self.nfd_state_to_gnn_input(state)
                graph = Data(
                    x=gnn_input.x,
                    edge_index=gnn_input.edge_indices,
                    y=0,  # dummy, we no longer use y
                    d_eval=dynamic_evaluator,
                )
                graphs.append(graph)

            tup = (
                graphs,
                r_data.good_idxs,
                r_data.maybe_bad_idxs,
                r_data.def_bad_idxs,
            )
            ret.append(tup)

        return (ret, "the other output should contain everything you need")

    def transform_prefschema_dataset(self, dataset: RawDataset, **kwargs) -> List[Data]:
        ret = []
        ys = []
        for problem, state_data_list in tqdm(dataset.items()):
            self.set_problem(problem)
            schemata = problem.schemata_names
            dynamic_evaluator = self.get_dynamic_evaluator()
            for state_data in state_data_list:
                if state_data.optimal_actions is None:
                    continue
                gnn_input = self.nfd_state_to_gnn_input(state_data.state)

                y_dict = optimal_actions_to_multilabel_schema(
                    schemata, state_data.optimal_actions
                )
                y = [y_dict[s] for s in sorted(schemata)]
                y = torch.tensor(y)
                ys.append(y)

                pyg_data = Data(
                    x=gnn_input.x,
                    edge_index=gnn_input.edge_indices,
                    y=y.unsqueeze(0),
                    d_eval=dynamic_evaluator,
                )
                ret.append(pyg_data)

        ## log info
        ys = torch.stack(ys)
        ys = torch.sum(ys, dim=0)
        print("Times schema is preferred:")
        print_mat([[k, ys[i].item()] for i, k in enumerate(sorted(schemata))])

        return ret

    def set_problem(self, problem: NumericProblem) -> None:
        graph = Graph(
            problem=problem,
            feature_generator=self.feature_generator,
            label_generator=self.label_generator,
            numeric_agnostic=self.numeric_agnostic,
        )
        self.graph = graph

    def set_static_vars(self, static_vars: List[str]) -> None:
        self.graph.update_from_statics(static_vars)

    def nfd_state_to_gnn_input(self, state: NumericState) -> GnnInput:
        return self.graph.state_to_gnn_input(state)

    def get_dynamic_evaluator(self) -> Optional[DynamicEvaluator]:
        assert self.graph is not None
        G = self.graph
        return DynamicEvaluator(G, self._opts)

    def dump(self) -> None:
        print(f"Representation configuration:")
        print(f"handles numerics: {not self.numeric_agnostic}")
        print(f"dynamic features: {self.dynamic_features}")
        print(f"# node features: {self.n_node_features}")
        print(f"# edge labels: {self.n_edge_labels}")
