from typing import Dict, Tuple, Union
from .base_wl import *


class ColourRefinement(WlAlgorithm):
    def __init__(self, **kwargs) -> None:
        super().__init__(**kwargs)
        self._k = 1

    def compute_histograms_helper(self, G: CGraph):
        cur_colours = {}
        histogram = {}

        def store_colour(colour):
            nonlocal histogram
            colour_hash = self._get_hash_value(colour)
            if colour_hash not in histogram:
                histogram[colour_hash] = 0
            histogram[colour_hash] += 1

        # collect initial colours
        for u in G.nodes:
            # initial colour is feature of the node
            colour = G.nodes[u]["x"]
            cur_colours[u] = self._get_hash_value(colour)
            # assert colour in self._hash and colour>=0, colour
            store_colour(colour)

        # WL iterations
        for itr in range(self.iterations):
            new_colours = {}
            for u in G.nodes:
                # edge label WL variant
                neighbour_colours = []
                for v in G[u]:
                    colour_node = cur_colours[v]
                    colour_edge = G.edges[(u, v)]["edge_label"]
                    neighbour_colours.append((colour_node, colour_edge))
                neighbour_colours = sorted(neighbour_colours)
                colour = tuple([cur_colours[u]] + neighbour_colours)
                new_colours[u] = self._get_hash_value(colour)
                store_colour(colour)

            cur_colours = new_colours

        return histogram
