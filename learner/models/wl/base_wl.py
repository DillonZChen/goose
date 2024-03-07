from itertools import combinations
import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import numpy as np
import networkx as nx
from abc import ABC, abstractmethod
from typing import List, Dict, Optional, Tuple, Union
from tqdm import tqdm
from representation import CGraph


Histogram = Dict[int, int]
NO_EDGE = -2

""" Base class for graph kernels """


class WlAlgorithm(ABC):
    def __init__(self, iterations: int, prune: int) -> None:
        self._train = True

        # hashes neighbour multisets of colours
        self._hash = {}

        # prune if self._train_histogram[col] <= count
        self._prune = prune

        # number of wl iterations
        self.iterations = iterations

        # counters during evaluation of hit and missed colours
        self._hit_colours = 0
        self._missed_colours = 0

        # for getting initial colours
        self._k = None

        return

    def compute_histograms(
        self, graphs: List[CGraph], return_ratio_seen_counts: bool
    ) -> Union[List[Histogram], Tuple[List[Histogram], List[float]]]:
        """Read graphs and return histograms and maybe ratio of seen counts.

        self._train value determines if new colours are stored or not
        """

        histograms = []
        ratio_seen_counts = []

        assert self._k is not None

        # get initial colours; quick and dirty repeated code
        if self._k == 1:
            for G in graphs:
                for u in G.nodes:
                    colour = G.nodes[u]["x"]
                    self._get_hash_value(colour)
        elif self._k == 2:
            for G in graphs:
                subsets = list(combinations(G.nodes, 2))

                # collect initial colours
                for subset in subsets:
                    u, v = subset

                    # initial colour is feature of the node
                    c_u = G.nodes[u]["x"]
                    c_v = G.nodes[v]["x"]
                    # graph is undirected so this equals (v, u) in G.edges
                    edge = (u, v)
                    is_edge = edge in G.edges
                    if is_edge:
                        edge_colour = G.edges[edge]["edge_label"]
                        assert edge_colour != NO_EDGE
                    else:
                        edge_colour = NO_EDGE  # no edge
                    # the more general k-wl algorithm colours by looking at colour-isomorphism
                    colour = (c_u, c_v, edge_colour)

                    self._get_hash_value(colour)

        else:
            raise NotImplementedError

        # compute colours and hashmap from training data
        for G in tqdm(graphs):
            G_seen_count = self._hit_colours
            G_unseen_count = self._missed_colours

            histogram = self.compute_histograms_helper(G)
            histograms.append(histogram)

            if not return_ratio_seen_counts:
                continue

            # ratio seen counts
            G_seen_count = self._hit_colours - G_seen_count
            G_unseen_count = self._missed_colours - G_unseen_count
            all_cnts = G_seen_count + G_unseen_count
            if all_cnts == 0:  # occurs during training
                ratio = 1
            else:
                ratio = G_seen_count / all_cnts
            ratio_seen_counts.append(ratio)

        if self._train and self._prune > 0:
            histograms = self._prune_hash(histograms)

        if return_ratio_seen_counts:
            return histograms, ratio_seen_counts
        else:
            return histograms

    @abstractmethod
    def compute_histograms_helper(self, G: CGraph) -> Histogram:
        raise NotImplementedError

    def get_hash(self) -> Dict[str, int]:
        """Return hash dictionary with compact keys for cpp"""
        ret = {}
        for k in self._hash:
            key = str(k)
            for symbol in [")", "(", " "]:
                key = key.replace(symbol, "")
            ret[key] = self._hash[k]
        return ret

    def _get_hash_value(self, colour) -> int:
        if isinstance(colour, tuple) and len(colour) == 1:
            colour = colour[0]
        if self._train:
            if colour not in self._hash:
                self._hash[colour] = len(self._hash)
                # print(colour)
            return self._hash[colour]
        else:
            if colour in self._hash:
                self._hit_colours += 1
                return self._hash[colour]
            else:
                self._missed_colours += 1
                return -1

    def _prune_hash(self, histograms):
        inverse_hash = {self._hash[col]: col for col in self._hash}

        # get histogram over all train graphs
        train_histogram = {}
        for G, histogram in histograms.items():
            for col_hash, cnt in histogram.items():
                col = inverse_hash[col_hash]
                if col not in train_histogram:
                    train_histogram[col] = 0
                train_histogram[col] += cnt

        # prune hash
        new_hash = {}
        old_colour_hash_to_new_hash = {}
        for col, old_col_hash in self._hash.items():
            if train_histogram[col] <= self._prune:
                del train_histogram[col]
                continue
            new_col_hash = len(new_hash)
            new_hash[col] = new_col_hash
            old_colour_hash_to_new_hash[old_col_hash] = new_col_hash
        self._hash = new_hash

        # prune from train set
        ret_histograms = []
        for histogram in histograms:
            new_histogram = {}
            for old_col_hash, cnt in histogram.items():
                if old_col_hash not in old_colour_hash_to_new_hash:
                    continue  # total count too small
                new_histogram[old_colour_hash_to_new_hash[old_col_hash]] = cnt
            ret_histograms.append(histogram)

        return ret_histograms

    def train(self) -> None:
        self._train = True

    def eval(self) -> None:
        self._train = False
        self._hit_colours = 0
        self._missed_colours = 0

    def get_hit_colours(self) -> int:
        return self._hit_colours

    def get_missed_colours(self) -> int:
        return self._missed_colours

    def get_x(
        self,
        graphs: List[CGraph],
        histograms: Optional[List[Histogram]] = None,
    ) -> np.array:
        """Explicit feature representation
        O(nd) time; n x d output
        """

        n = len(graphs)
        d = len(self._hash)
        X = np.zeros((n, d))

        if histograms is None:
            histograms = self.compute_histograms(
                graphs, return_ratio_seen_counts=False
            )

        for i, histogram in enumerate(histograms):
            for j in histogram:
                if 0 <= j and j < d:
                    X[i][j] = histogram[j]

        return X

    def get_k(
        self, graphs: List[CGraph], histograms: List[Histogram]
    ) -> np.array:
        """Implicit feature representation
        O(n^2d) time; n x n output
        """

        n = len(graphs)
        K = np.zeros((n, n))
        for i in range(n):
            for j in range(i, n):
                k = 0

                histogram_i = histograms[i]
                histogram_j = histograms[j]

                common_colours = set(histogram_i.keys()).intersection(
                    set(histogram_j.keys())
                )
                for c in common_colours:
                    k += histogram_i[c] * histogram_j[c]

                K[i][j] = k
                K[j][i] = k

        return K

    def update_iterations(self, iterations):
        self.iterations = iterations

    @property
    def n_colours_(self) -> int:
        return len(self._hash)
