import time
from .base_kernel import *


class WeisfeilerLehmanKernel(Kernel):
  def __init__(self, iterations: int) -> None:
    super().__init__()
    self.f = {}  # hashmap
    self.iterations = iterations  # number of wl iterations

  def read_data(self, graphs: Iterable[nx.Graph]) -> None:
    """ Read data and precompute the hash function """

    t = time.time()

    for G in graphs:
      cur_colours = {}

      # collect initial colours
      for u in G.nodes:
        colour = G.nodes[u]["colour"]
        if colour not in self.f:
          self.f[colour] = len(self.f)
        cur_colours[u] = self.f[colour]

      # wl iterations
      for _ in range(1,self.iterations+1):
        new_colours = {}
        for u in G.nodes:
          colour = tuple([cur_colours[u]] + sorted([cur_colours[v] for v in G[u]]))
          if colour not in self.f:
            self.f[colour] = len(self.f)
          new_colours[u] = self.f[colour]
        cur_colours = new_colours

    t = time.time() - t
    print(f"Initialised WL for {len(graphs)} graphs in {t:.2f}s")
    print(f"Collected {len(self.f)} colours over {sum(len(G.nodes) for G in graphs)} nodes")
    return
