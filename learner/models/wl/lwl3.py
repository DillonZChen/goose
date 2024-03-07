from typing import Optional, Dict
from itertools import combinations
from tqdm import tqdm
from .base_wl import *

""" 3-LWL """
# this class can be easily changed into k-LWL


class LWL3(WlAlgorithm):
    def __init__(self, **kwargs) -> None:
        super().__init__(**kwargs)

    def compute_histograms_helper(self, G: CGraph):
        # probably wrong after updates on other code, see lwl2 for help to update
        raise NotImplementedError
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

        subsets = list(combinations(G.nodes, 3))

        # collect initial colours
        for subset in subsets:
            u, v, w = subset

            # initial colour is feature of the node
            c_u = G.nodes[u]["x"]
            c_v = G.nodes[v]["x"]
            c_w = G.nodes[w]["x"]
            # graph is undirected so this equals (v, u) in G.edges
            edges = [(u, v), (v, w), (v, u)]
            node_colours = sorted([c_u, c_v, c_w])
            edge_colours = []
            for edge in edges:
                if edge in G.edges:
                    edge_colour = G.edges[edge]["edge_label"]
                    assert edge_colour != NO_EDGE
                else:
                    edge_colour = NO_EDGE  # no ed
                edge_colours.append(edge_colour)
            edge_colours = sorted(edge_colours)

            colour = tuple(node_colours + edge_colours)

            cur_colours[subset] = self._get_hash_value(colour)
            assert colour in self._hash, colour
            store_colour(colour)

        # WL iterations
        for itr in range(self.iterations):
            new_colours = {}
            for subset in subsets:
                u, v, w = subset

                # k-wl does not care about graph structure after initial colours
                neighbour_nodes = set()
                for node in subset:
                    neighbour_nodes = neighbour_nodes.union(set(G[node]))
                neighbour_nodes = neighbour_nodes.difference(set(subset))

                neighbour_colours = []
                for x in neighbour_nodes:
                    subset1 = tuple(
                        sorted((u, v, x))
                    )  # tuple(sorted(.)) is a hashable
                    subset2 = tuple(
                        sorted((u, x, w))
                    )  # tuple(sorted(.)) is a hashable
                    subset3 = tuple(
                        sorted((x, v, w))
                    )  # tuple(sorted(.)) is a hashable
                    colour = tuple(
                        sorted(
                            (
                                cur_colours[subset1],
                                cur_colours[subset2],
                                cur_colours[subset3],
                            )
                        )
                    )
                    neighbour_colours.append(colour)

                # equation-wise, neighbour colours is a multiset of colours
                neighbour_colours = sorted(neighbour_colours)
                colour = tuple([cur_colours[subset]] + neighbour_colours)
                new_colours[subset] = self._get_hash_value(colour)
                store_colour(colour)

            cur_colours = new_colours

        return histogram
