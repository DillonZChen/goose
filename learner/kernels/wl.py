import time
from .base_kernel import *


class WeisfeilerLehmanKernel(Kernel):
  def __init__(self, iterations: int) -> None:
    super().__init__()

    # hashes neighbour multisets of colours; also acts as colour to explicit feature index
    self._hash = {}

    # number of wl iterations
    self.iterations = iterations

  def read_train_data(self, graphs: CGraph) -> None:
    """ Read data and precompute the hash function """

    t = time.time()
    self._train_data_colours = {}

    # initial run to compute colours and hashmap
    for G in graphs:
      cur_colours = {}
      histogram = {}

      # collect initial colours
      for u in G.nodes:

        # initial colour is feature of the node
        colour = G.nodes[u]["colour"]

        # check if colour in hash to compress
        if colour not in self._hash:
          self._hash[colour] = len(self._hash)
        cur_colours[u] = self._hash[colour]

        # store histogram throughout all iterations
        if colour not in histogram:
          histogram[colour] = 0
        histogram[colour] += 1

      # WL iterations
      for _ in range(self.iterations):
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

          # check if colour in hash to compress
          if colour not in self._hash:
            self._hash[colour] = len(self._hash)
          new_colours[u] = self._hash[colour]

          # store histogram throughout all iterations
          if colour not in histogram:
            histogram[colour] = 0
          histogram[colour] += 1
        cur_colours = new_colours
      
      # store histogram of graph colours over *all* iterations
      self._train_data_colours[G] = histogram

    t = time.time() - t
    print(f"Initialised WL for {len(graphs)} graphs in {t:.2f}s")
    print(f"Collected {len(self._hash)} colours over {sum(len(G.nodes) for G in graphs)} nodes")
    return

  def get_x(self, graphs: CGraph) -> np.array:
    """ Explicit feature representation
        O(nd) time; n x d output 
    """
    n = len(graphs)
    d = len(self._hash)
    X = np.zeros((n, d))
    for i, G in enumerate(graphs):
      histogram = self._train_data_colours[G]
      for colour in histogram:
        j = self._hash[colour]
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
  