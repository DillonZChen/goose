from abc import abstractmethod
from argparse import Namespace
from typing import List

from learner.dataset.ranking_data import RankingData
from learner.dataset.raw_dataset import RawDataset


class Representation:
    def __init__(self, opts: Namespace) -> None:
        self._opts = opts
        self.domain_pddl = opts.domain_pddl

    @abstractmethod
    def transform_heuristic_dataset(self, dataset: RawDataset, **kwargs):
        raise NotImplementedError

    @abstractmethod
    def transform_deadend_dataset(self, dataset: RawDataset, **kwargs):
        raise NotImplementedError

    @abstractmethod
    def transform_ranking_dataset(self, ranking_data: List[RankingData], **kwargs):
        raise NotImplementedError

    @abstractmethod
    def transform_prefschema_dataset(self, dataset: RawDataset, **kwargs):
        raise NotImplementedError

    @abstractmethod
    def set_static_vars(self, static_vars: List[str]) -> None:
        raise NotImplementedError

    @abstractmethod
    def dump(self) -> None:
        raise NotImplementedError
