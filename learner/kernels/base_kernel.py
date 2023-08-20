import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import numpy as np
import networkx as nx
from abc import ABC, abstractmethod
from typing import List
from representation import CGraph


""" Base class for graph kernels """
class Kernel(ABC):
  def __init__(self) -> None:
    return

  @abstractmethod
  def read_train_data(self, graphs: CGraph) -> None:
    raise NotImplementedError

  @abstractmethod
  def get_x(self, graphs: CGraph) -> np.array:
    raise NotImplementedError
  
  @abstractmethod
  def get_k(self, graphs: CGraph) -> np.array:
    raise NotImplementedError