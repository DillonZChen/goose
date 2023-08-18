import numpy as np
import networkx as nx
from abc import ABC, abstractmethod
from typing import List


""" Base class for graph kernels """
class Kernel(ABC):
  def __init__(self) -> None:
    return

  @abstractmethod
  def read_train_data(self, graphs: List[nx.Graph]) -> None:
    raise NotImplementedError

  @abstractmethod
  def get_x(self, graphs: List[nx.Graph]) -> np.array:
    raise NotImplementedError
  
  @abstractmethod
  def get_k(self, graphs: List[nx.Graph]) -> np.array:
    raise NotImplementedError