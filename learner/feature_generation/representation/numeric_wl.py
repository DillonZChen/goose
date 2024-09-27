from dataclasses import dataclass
from typing import Dict, List, Tuple, Union

import numpy as np
from tqdm import tqdm

from learner.feature_generation.representation.graph import (CatFeature, ConFeature,
                                                             FeatureGenerator, Graph,
                                                             Node)

Hash = Dict[str, int]

CONCAT = 2


@dataclass
class NwlOutput:
    cat_histogram: Dict[CatFeature, int]
    con_histogram: Dict[CatFeature, List[float]]


class NumericWl:
    def __init__(self, cat_iterations: int, con_iterations: int, graph_feat_gen: FeatureGenerator, numeric: bool):
        self._hash = {}
        self._train = True
        self.cat_iterations = cat_iterations
        self.con_iterations = con_iterations
        assert cat_iterations >= con_iterations, f"{cat_iterations} < {con_iterations}"
        self.graph_feat_gen = graph_feat_gen
        self.n_cat_features = None
        self.n_con_features = None
        self.n_features = None
        self._itr_to_cats = {i: set() for i in range(cat_iterations + 1)}
        self._init_init_collected = set()
        self._numeric = numeric

    def train(self) -> None:
        self._train = True

    def eval(self) -> None:
        self._train = False

    def get_hash(self) -> Dict[str, int]:
        """Return hash dictionary with compact keys for cpp"""
        ret = {}
        for k in self._hash:
            key = str(k)
            for symbol in [")", "(", " "]:
                key = key.replace(symbol, "")
            ret[key] = self._hash[k]
        # for k, v in ret.items():
        #     print(k, v, flush=True)
        return ret

    def _get_hash_value(self, colour: Tuple[CatFeature], itr: int) -> int:
        """Get compressed colours, and stores if training"""
        if isinstance(colour, tuple) and len(colour) == 1:
            # python does some weird things with singleton tuples
            # this may also happen due to singleton nodes
            colour = colour[0]
        if self._train:
            if colour not in self._hash:
                self._hash[colour] = len(self._hash)
            self._itr_to_cats[itr].add(self._hash[colour])
            return self._hash[colour]
        else:
            if colour in self._hash:
                return self._hash[colour]
            else:
                return -1

    def compute(self, graphs: List[Graph]) -> List[np.array]:
        # store initial colours from all graphs
        for graph in graphs:
            for u in graph.nodes:
                colour = graph.x_cat[u]
                self._get_hash_value(colour, itr=0)

        # perform main chunk of WL
        outputs = []
        for graph in graphs:
            outputs.append(self.compute_histograms(graph))

        # analyse and postprocess features
        if self.n_cat_features is None:
            assert self._train
            self.n_cat_features = len(self._hash)
            if self._numeric:
                self.n_con_features = 1 * self.n_cat_features
            else:
                self.n_con_features = 0
            self.n_features = self.n_cat_features + self.n_con_features

        # convert features into numpy array
        X = []
        for graph, output in zip(graphs, outputs):
            cat_histogram = output.cat_histogram
            con_histogram = output.con_histogram
            x_cat = np.zeros(self.n_cat_features)
            x_sum = np.zeros(self.n_cat_features)
            # x_max = np.zeros(self.n_cat_features)

            for cat, count in cat_histogram.items():
                x_cat[cat] = count
            for cat, cons in con_histogram.items():
                x_sum[cat] = sum(cons)  # pool with sum
                # x_max[cat] = max(cons)  # pool with max  ## TODO: try out
            
            if self._numeric:
                x = np.concatenate([x_cat, x_sum])
                assert len(x) == self.n_features, f"{len(x)} != {self.n_features}"
            else:
                x = x_cat
            X.append(x)

        return X

    def compute_histograms(self, graph: Graph) -> NwlOutput:
        cur_cat = np.array(graph.x_cat)
        cur_con = np.array([v for v in graph.x_con])

        cat_histogram = {}
        con_histogram = {}

        def store(
            cat_aggr: Tuple[CatFeature], con_aggr: List[ConFeature], itr: int
        ) -> CatFeature:
            nonlocal cat_histogram

            cat_aggr = self._get_hash_value(cat_aggr, itr=itr)
            con_aggr = max(con_aggr) if len(con_aggr) > 0 else 0  # max aggregator
            if cat_aggr == -1:
                return -1, con_aggr

            if cat_aggr not in cat_histogram:
                cat_histogram[cat_aggr] = 0
            if cat_aggr not in con_histogram and itr <= self.con_iterations:
                con_histogram[cat_aggr] = []

            cat_histogram[cat_aggr] += 1
            if itr <= self.con_iterations:
                con_histogram[cat_aggr].append(con_aggr)

            return cat_aggr, con_aggr

        # collect initial colours
        for u in graph.nodes:
            if self._train:
                self._init_init_collected.add(cur_cat[u])
            cat = cur_cat[u]  # =cat_aggr, as this gets compressed despite being int
            con = [cur_con[u]]
            cat_hash, con_hash = store(cat_aggr=cat, con_aggr=con, itr=0)
            cur_cat[u] = cat_hash
            cur_con[u] = con_hash

        ## main loop
        for itr in range(self.cat_iterations):
            # print(f"Iteration {_}", flush=True)
            new_cat = np.zeros_like(cur_cat)
            new_con = np.zeros_like(cur_con)

            for u in graph.nodes:
                neighbours = graph.neighbours[u]
                neighbour_cats = set()
                neighbour_cons = [cur_con[u]]
                for v, edge_label in neighbours:
                    cat_v = cur_cat[v]
                    neighbour_cats.add((cat_v, edge_label))
                    # neighbour_cons.append(cur_con[v])
                neighbour_cats = sorted(neighbour_cats)

                ## same as canonical WL update
                cat_aggr = tuple([cur_cat[u]] + neighbour_cats)
                ## throw away self as it is stored earlier
                con_aggr = neighbour_cons

                ret = store(cat_aggr, con_aggr, itr + 1)
                cat_hash, con_hash = ret

                # print(u, "->", cat_hash, "->", str(cat_aggr).replace(' ', '').replace("(", '').replace(")", '').replace("[", '').replace("]", ''), flush=True)

                # print(u, "->", flush=True)
                # for j, c in enumerate(con_aggr):
                #     if c - round(c) == 0:
                #         c = int(c)
                #     print(j, c, flush=True)

                new_cat[u] = cat_hash
                new_con[u] = con_hash

            cur_cat = new_cat
            cur_con = new_con

        return NwlOutput(cat_histogram, con_histogram)

    def dump(self):
        print("Cat iterations:", self.cat_iterations)
        print("Con iterations:", self.con_iterations)
        print("Number of categorical features:", self.n_cat_features)
        for itr, cats in self._itr_to_cats.items():
            print(f"Iteration {itr} categorical features: {len(cats)}")
        # print(self._init_init_collected)
        print("Number of continuous features:", self.n_con_features)
        print("Total number of features:", self.n_features)
