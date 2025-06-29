from learning.dataset.container.base_dataset import Dataset
from wlplan.data import ProblemDataset
from wlplan.planning import Domain


class CostToGoDataset(Dataset):
    def __init__(self, wlplan_domain: Domain, data: list[ProblemDataset], y: list[float]):
        super().__init__(wlplan_domain, data)
        self._y = y

    @property
    def y(self) -> list[float]:
        return self._y
