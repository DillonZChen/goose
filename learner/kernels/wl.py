import time
from .base_kernel import *


class WeisfeilerLehmanKernel(Kernel):
  def __init__(self, iterations: int) -> None:
    super().__init__()

    # hashes neighbour multisets of colours; also acts as colour to explicit feature index
    self._hash = {}

    # number of wl iterations
    self.iterations = iterations

  def read_train_data(self, graphs: List[nx.Graph]) -> None:
    """ Read data and precompute the hash function """

    t = time.time()
    self._train_data_colours = {}

    # initial run to compute colours and hashmap
    for G in graphs:
      cur_colours = {}
      histogram = {}

      # collect initial colours
      for u in G.nodes:
        colour = G.nodes[u]["colour"]
        if colour not in self._hash:
          self._hash[colour] = len(self._hash)
        if colour not in histogram:
          histogram[colour] = 0
        histogram[colour] += 1
        cur_colours[u] = self._hash[colour]

      # wl iterations
      for _ in range(self.iterations):
        new_colours = {}
        for u in G.nodes:
          colour = tuple([cur_colours[u]] + sorted([cur_colours[v] for v in G[u]]))
          if colour not in self._hash:
            self._hash[colour] = len(self._hash)
          new_colours[u] = self._hash[colour]
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

  def get_x(self, graphs: List[nx.Graph]) -> np.array:
    n = len(graphs)
    d = len(self._hash)
    X = np.zeros((n, d))
    for i, G in enumerate(graphs):
      histogram = self._train_data_colours[G]
      for colour in histogram:
        j = self._hash[colour]
        X[i][j] = histogram[colour]
    return X
  
  def get_k(self, graphs: List[nx.Graph]) -> np.array:
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
  