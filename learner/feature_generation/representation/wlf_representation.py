from typing import Dict, List, Tuple

import numpy as np
from tqdm import tqdm

from learner.dataset.dataset import ALL_KEY
from learner.dataset.preferred_schema_dataset import \
    optimal_actions_to_multilabel_schema
from learner.dataset.ranking_data import RankingData
from learner.dataset.raw_dataset import RawDataset
from learner.feature_generation.representation.graph import FeatureGenerator, Graph
from learner.feature_generation.representation.numeric_wl import NumericWl
from learner.problem.numeric_problem import NumericProblem
from learner.problem.numeric_state import NumericState
from learner.representation import Representation
from util.statistics import dump_several_stats, print_mat
from util.timer import TimerContextManager

XyData = List[Tuple[np.array, Dict[str, float]]]


class CCwlRepresentation(Representation):
    def __init__(self, opts):
        super().__init__(opts)
        self._opts = opts
        self.feature_generator = FeatureGenerator(opts.domain_pddl)
        self.numeric_wl = NumericWl(
            cat_iterations=opts.cat_iterations,
            con_iterations=opts.con_iterations,
            graph_feat_gen=self.feature_generator,
            numeric=opts.numeric,
        )
        self.graph = None

    def transform_heuristic_dataset(self, dataset: RawDataset, **kwargs) -> XyData:
        graphs_trace: List[Graph] = []
        graphs_succs: List[Graph] = []
        y_trace = []
        y_succs = []
        for problem, state_data_list in tqdm(dataset.items()):
            self.set_problem(problem)
            for state_data in state_data_list:
                if not state_data.description == "opt":
                    continue
                graph = self.state_to_graph(state_data.state)
                if state_data.optimal_actions is not None:
                    graphs_trace.append(graph)
                    y_trace.append({ALL_KEY: state_data.heuristic})
                else:
                    graphs_succs.append(graph)
                    y_succs.append({ALL_KEY: state_data.heuristic})

        with TimerContextManager("collecting features from training set"):
            self.numeric_wl.train()
            X_trace = self.compute_features(graphs_trace)
            self.numeric_wl.eval()
            # do not collect colours from successors since there's too many of them
            X_succs = self.compute_features(graphs_succs)
            self.numeric_wl.train()

        X = X_trace + X_succs
        y = y_trace + y_succs

        ## log info
        dump_several_stats(([d[ALL_KEY] for d in y], f"target"))

        return list(zip(X, y))

    def transform_ranking_dataset(
        self, ranking_data: List[RankingData], **kwargs
    ) -> Tuple[XyData, Dict[int, List[int]]]:
        graphs: List[Graph] = []
        ys = []

        trace_idx = []
        succs_idx = []

        ret_succ_groups: List[Tuple[List[int], List[int], List[int]]] = []

        for r_data in ranking_data:
            self.set_problem(r_data.problem)
            offset = len(graphs)

            for state in r_data.states:
                graph = self.state_to_graph(state)
                graphs.append(graph)
                ys.append({ALL_KEY: 0})  # dummy, we no longer use y

            good_idxs = [offset + i for i in r_data.good_idxs]
            maybe_bad_idxs = [offset + i for i in r_data.maybe_bad_idxs]
            def_bad_idxs = [offset + i for i in r_data.def_bad_idxs]

            for i in good_idxs:
                trace_idx.append(i)
            for i in maybe_bad_idxs:
                succs_idx.append(i)
            for i in def_bad_idxs:
                succs_idx.append(i)

            tup = (good_idxs, maybe_bad_idxs, def_bad_idxs)
            ret_succ_groups.append(tup)

        with TimerContextManager("collecting features from training set"):
            self.numeric_wl.train()
            # start to collect colours from only trace states
            graphs_trace = [graphs[i] for i in trace_idx]
            X_trace = self.compute_features(graphs_trace)
            # do not collect colours from successors since there's too many of them
            self.numeric_wl.eval()
            graphs_succs = [graphs[i] for i in succs_idx]
            X_succs = self.compute_features(graphs_succs)
            self.numeric_wl.train()
        idx = 0
        trace_ptr = 0
        succs_ptr = 0
        trace_idx = sorted(trace_idx)
        succs_idx = sorted(succs_idx)
        X = []
        while idx < len(graphs):
            if trace_ptr < len(X_trace) and idx == trace_idx[trace_ptr]:
                X.append(X_trace[trace_ptr])
                idx += 1
                trace_ptr += 1
            else:
                assert succs_ptr < len(X_succs) and idx == succs_idx[succs_ptr]
                X.append(X_succs[succs_ptr])
                idx += 1
                succs_ptr += 1

        return list(zip(X, ys)), ret_succ_groups

    def transform_deadend_dataset(self, dataset: RawDataset, **kwargs) -> XyData:
        graphs_trace: List[Graph] = []
        graphs_succs: List[Graph] = []
        y_trace = []
        y_succs = []
        for problem, state_data_list in tqdm(dataset.items()):
            self.set_problem(problem)
            for state_data in state_data_list:
                if state_data.description == "unsolved":
                    continue
                y = state_data.description == "dead"
                graph = self.state_to_graph(state_data.state)
                if state_data.optimal_actions is not None:
                    graphs_trace.append(graph)
                    y_trace.append({ALL_KEY: y})
                else:
                    graphs_succs.append(graph)
                    y_succs.append({ALL_KEY: y})

        with TimerContextManager("collecting features from training set"):
            self.numeric_wl.train()
            X_trace = self.compute_features(graphs_trace)
            self.numeric_wl.eval()
            # do not collect colours from successors since there's too many of them
            X_succs = self.compute_features(graphs_succs)
            self.numeric_wl.train()

        X = X_trace + X_succs
        y = y_trace + y_succs

        ## log info
        n = len(y)
        ones = int(sum(d[ALL_KEY] for d in y))
        zeros = n - ones
        print(f"{ones} deadends")
        print(f"{zeros} non-deadends")

        return list(zip(X, y))

    def transform_prefschema_dataset(self, dataset: RawDataset, **kwargs) -> XyData:
        graphs = []
        ys = []
        for problem, state_data_list in tqdm(dataset.items()):
            self.set_problem(problem)
            schemata = problem.schemata_names
            for state_data in state_data_list:
                if not state_data.description == "opt":
                    continue
                graph = self.state_to_graph(state_data.state)
                if state_data.optimal_actions is not None:
                    graphs.append(graph)

                    y_dict = optimal_actions_to_multilabel_schema(
                        schemata, state_data.optimal_actions
                    )

                    ys.append(y_dict)

        with TimerContextManager("collecting features from training set"):
            self.numeric_wl.train()
            X = self.compute_features(graphs)

        ## log info
        schemata_cnt = {s: 0 for s in schemata}
        for y in ys:
            for k in schemata:
                schemata_cnt[k] += y[k]
        print("Times schema is preferred:")
        print_mat([[k, schemata_cnt[k]] for k in sorted(schemata)])

        return list(zip(X, ys))

    def set_problem(self, problem: NumericProblem) -> None:
        self.graph = Graph(problem, self.feature_generator)

    def set_static_vars(self, static_vars: List[str]) -> None:
        self.graph.update_from_statics(static_vars)

    def state_to_graph(self, state: NumericState) -> Graph:
        return self.graph.state_to_graph(state)

    def compute_features(self, graphs: List[Graph]) -> List[np.array]:
        return self.numeric_wl.compute(graphs)

    def dump(self):
        print("Representation information:")
        self.feature_generator.dump()
        self.numeric_wl.dump()
