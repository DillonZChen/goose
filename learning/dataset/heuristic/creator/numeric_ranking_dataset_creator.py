import wlplan
from learning.dataset.heuristic.container.base_dataset import Dataset
from learning.dataset.heuristic.container.ranking_dataset import RankingDataset, RankingGroup
from wlplan.data import ProblemDataset

from .numeric_dataset_creator import NumericDatasetCreator


class NumericRankingDatasetFromPlans(NumericDatasetCreator):
    def get_dataset(self) -> Dataset:
        wlplan_data = []
        y: list[RankingGroup] = []
        states_added = 0

        for problem_pddl, plan_file in self._get_problem_iterator():
            wlplan_problem = wlplan.planning.parse_problem(self.domain_pddl, problem_pddl)
            wlplan_states = []
            info = self._get_nfd_info(problem_pddl, plan_file)
            ranking_groups = info["ranking_groups"]

            for ranking_group in ranking_groups:
                good_group = []
                maybe_group = []
                bad_group = []

                for state in ranking_group["good_group"]:
                    good_group.append(states_added)
                    states_added += 1
                    wlplan_states.append(state)

                for state in ranking_group["maybe_group"]:
                    maybe_group.append(states_added)
                    states_added += 1
                    wlplan_states.append(state)

                for state in ranking_group["bad_group"]:
                    bad_group.append(states_added)
                    states_added += 1
                    wlplan_states.append(state)

                y.append(RankingGroup(good_group, maybe_group, bad_group))

            wlplan_data.append((wlplan_problem, wlplan_states))

        data = []
        for problem, states in wlplan_data:
            data.append(ProblemDataset(problem=problem, states=states))
        dataset = RankingDataset(wlplan_domain=self._wlplan_domain, data=data, y=y)

        return dataset
