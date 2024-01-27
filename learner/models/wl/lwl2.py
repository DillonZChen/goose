from typing import Optional, Dict
from itertools import combinations
from tqdm import tqdm
from .base_wl import *

""" 2-LWL """
# this class can be easily changed into k-LWL


class LWL2(WlAlgorithm):
    def __init__(self, **kwargs) -> None:
        super().__init__(**kwargs)

    def compute_histograms_helper(self, G: CGraph):
        cur_colours = {}
        histogram = {}

        def store_colour(colour):
            nonlocal histogram
            colour_hash = self._get_hash_value(colour)
            if colour_hash not in histogram:
                histogram[colour_hash] = 0
            histogram[colour_hash] += 1

        n_nodes = len(G.nodes)
        assert set(G.nodes) == set(range(n_nodes))

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

            cur_colours[subset] = self._get_hash_value(colour)
            # assert colour in self._hash, colour
            store_colour(colour)

        # WL iterations
        for itr in range(self.iterations):
            new_colours = {}
            for subset in subsets:
                u, v = subset

                # k-wl does not care about graph structure after initial colours
                neighbour_nodes = set(G[u]).union(set(G[v])).difference({u, v})
                neighbour_colours = []
                for w in neighbour_nodes:
                    subset1 = tuple(
                        sorted((u, w))
                    )  # tuple(sorted(.)) is a hashable
                    subset2 = tuple(sorted((v, w)))
                    colour = tuple(
                        sorted((cur_colours[subset1], cur_colours[subset2]))
                    )
                    neighbour_colours.append(colour)

                # equation-wise, neighbour colours is a multiset of colours
                neighbour_colours = sorted(neighbour_colours)
                colour = tuple([cur_colours[subset]] + neighbour_colours)
                new_colours[subset] = self._get_hash_value(colour)
                store_colour(colour)

            cur_colours = new_colours

        return histogram
