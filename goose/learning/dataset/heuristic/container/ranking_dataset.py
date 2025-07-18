from dataclasses import dataclass

from goose.learning.dataset.heuristic.container.base_dataset import Dataset
from wlplan.data import ProblemDataset
from wlplan.planning import Domain


@dataclass
class RankingGroup:
    """A tuple of list of indices representing states segragated into good, maybe, and bad groups.

    RankingGroups are currently only computed from optimal plans (see dataset_factory._RankingDatasetFromPlans).
    A plan induces state-action pairs (s_i, a_i) where action a_i is an optimal action in state s_i and
    leads to state s_{i+1}.

    RankingGroup are computed from state-action pairs (s, a) with
        - good_group = [s*]
        - maybe_group = siblings(s*)
        - bad_group = [s]
    where
        - s* = apply(a, s)
        - successors(s) = list of successor states from s
        - siblings(s*) = successors(s) with s* removed
    """

    good_group: list[int]
    maybe_group: list[int]
    bad_group: list[int]

    def get_hashable_repr(self):
        return (tuple(self.good_group), tuple(self.maybe_group), tuple(self.bad_group))


class RankingDataset(Dataset):
    def __init__(self, wlplan_domain: Domain, data: list[ProblemDataset], y: RankingGroup):
        super().__init__(wlplan_domain, data)
        self._y = y

    @property
    def y(self) -> RankingGroup:
        return self._y
