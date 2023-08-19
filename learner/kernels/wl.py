import time
from .base_kernel import *


class WeisfeilerLehmanKernel(Kernel):
  def __init__(self, iterations: int, all_colours: bool) -> None:
    super().__init__()

    # hashes neighbour multisets of colours; same as self._representation if all_colours
    self._hash = {}

    # option for returning only final WL iteration
    self._representation = {}

    # number of wl iterations
    self.iterations = iterations

    # collect colours from all iterations or only final
    self.all_colours = all_colours

  def _get_hash_value(self, colour) -> int:
    if colour not in self._hash:
      self._hash[colour] = len(self._hash)
    return self._hash[colour]

  def read_train_data(self, graphs: CGraph) -> None:
    """ Read data and precompute the hash function """

    t = time.time()
    self._train_data_colours = {}

    # compute colours and hashmap from training data
    for G in graphs:
      cur_colours = {}
      histogram = {}

      def store_colour(colour):
        nonlocal histogram
        if colour not in self._representation:
          self._representation[colour] = len(self._representation)
        if colour not in histogram:
          histogram[colour] = 0
        histogram[colour] += 1

      # collect initial colours
      for u in G.nodes:

        # initial colour is feature of the node
        colour = G.nodes[u]["colour"]
        cur_colours[u] = self._get_hash_value(colour)

        # store histogram for all iterations or only last
        if self.all_colours or self.iterations == 0:
          store_colour(colour)

      # WL iterations
      for itr in range(self.iterations):
        new_colours = {}
        for u in G.nodes:

          # edge label WL variant
          neighbour_colours = []
          for v in G[u]:
            colour_node = cur_colours[v]
            colour_edge = G.edges[(u,v)]["edge_label"]
            neighbour_colours.append((colour_node, colour_edge))
          neighbour_colours = sorted(neighbour_colours)
          colour = tuple([cur_colours[u]] + neighbour_colours)
          new_colours[u] = self._get_hash_value(colour)

          # store histogram for all iterations or only last
          if self.all_colours or itr == self.iterations - 1:
            store_colour(colour)
        cur_colours = new_colours
      
      # store histogram of graph colours
      self._train_data_colours[G] = histogram

    if self.all_colours:
      self._representation = self._hash

    t = time.time() - t
    print(f"Initialised WL for {len(graphs)} graphs in {t:.2f}s")
    print(f"Collected {len(self._hash)} colours over {sum(len(G.nodes) for G in graphs)} nodes")
    return

  def get_x(self, graphs: CGraph) -> np.array:
    """ Explicit feature representation
        O(nd) time; n x d output 
    """
    n = len(graphs)
    d = len(self._representation)
    X = np.zeros((n, d))
    for i, G in enumerate(graphs):
      histogram = self._train_data_colours[G]
      for colour in histogram:
        j = self._representation[colour]
        X[i][j] = histogram[colour]
    return X
  
  def get_k(self, graphs: CGraph) -> np.array:
    """ Implicit feature representation
        O(n^2d) time; n x n output 
    """
    n = len(graphs)
    K = np.zeros((n, n))
    for i in range(n):
      for j in range(i, n):
        k = 0

        histogram_i = self._train_data_colours[graphs[i]]
        histogram_j = self._train_data_colours[graphs[j]]

        common_colours = set(histogram_i.keys()).intersection(set(histogram_j.keys()))
        for c in common_colours:
          k += histogram_i[c] * histogram_j[c]

        K[i][j] = k
        K[j][i] = k
    return K
  