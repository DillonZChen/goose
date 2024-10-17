from abc import ABC, abstractmethod

from wlplan.data import Dataset as WLPlanDataset
from wlplan.data import ProblemStates
from wlplan.planning import Domain


class Dataset(ABC):
    def __init__(self, wlplan_domain: Domain, data: list[ProblemStates]):
        self._domain = wlplan_domain
        self._data = data
        self._wlplan_dataset = WLPlanDataset(wlplan_domain, data)

    @property
    def data(self) -> list[ProblemStates]:
        return self._data

    @property
    def domain(self) -> Domain:
        return self._domain

    @property
    def wlplan_dataset(self) -> list[ProblemStates]:
        return self._wlplan_dataset

    @property
    @abstractmethod
    def y(self):
        pass
