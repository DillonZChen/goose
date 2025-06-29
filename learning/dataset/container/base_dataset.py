from abc import ABC, abstractmethod

from wlplan.data import DomainDataset, ProblemDataset
from wlplan.planning import Domain


class Dataset(ABC):
    def __init__(self, wlplan_domain: Domain, data: list[ProblemDataset]):
        self._domain = wlplan_domain
        self._data = data
        self._wlplan_dataset = DomainDataset(wlplan_domain, data)

    @property
    def data(self) -> list[ProblemDataset]:
        return self._data

    @property
    def domain(self) -> Domain:
        return self._domain

    @property
    def wlplan_dataset(self) -> DomainDataset:
        return self._wlplan_dataset

    @property
    @abstractmethod
    def y(self):
        pass

    def __len__(self):
        return len(self._data)
