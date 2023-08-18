import networkx as nx
from abc import ABC, abstractmethod
from typing import Iterable


""" Base class for graph kernels """
class Kernel(ABC):
  def __init__(self) -> None:
    return

  @abstractmethod
  def read_data(self, graphs: Iterable[nx.Graph]) -> None:
    raise NotImplementedError
