import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import numpy as np
import networkx as nx
from abc import ABC, abstractmethod
from typing import List, Dict
from representation import CGraph


Histogram = Dict[int, int]

""" Base class for graph kernels """


class Kernel(ABC):
    def __init__(self) -> None:
        self._train = True
        return

    def train(self) -> None:
        self._train = True

    def eval(self) -> None:
        self._train = False

    @abstractmethod
    def get_x(self, graphs: CGraph) -> np.array:
        raise NotImplementedError

    @abstractmethod
    def get_k(self, graphs: CGraph) -> np.array:
        raise NotImplementedError
