import wlplan
from learning.dataset.container.base_dataset import Dataset
from learning.dataset.container.cost_to_go_dataset import CostToGoDataset
from wlplan.data import ProblemStates

from .numeric_dataset_creator import NumericDatasetCreator


class NumericCostToGoDatasetFromPlans(NumericDatasetCreator):
    def get_dataset(self) -> Dataset:
        wlplan_data = []
        y = []

        for problem_pddl, plan_file in self._get_problem_iterator():
            wlplan_problem = wlplan.planning.parse_problem(self.domain_pddl, problem_pddl)
            wlplan_states = []
            info = self._get_nfd_info(problem_pddl, plan_file)
            ranking_groups = info["ranking_groups"]

            h_opt = len(ranking_groups)

            for ranking_group in ranking_groups:  # skips the goal state
                group = ranking_group["bad_group"]  # i.e. the parent
                assert len(group) == 1
                state = group[0]
                wlplan_states.append(state)
                y.append(h_opt)
                h_opt -= 1

            wlplan_data.append((wlplan_problem, wlplan_states))

        data = []
        for problem, states in wlplan_data:
            data.append(ProblemStates(problem=problem, states=states))
        dataset = CostToGoDataset(wlplan_domain=self._wlplan_domain, data=data, y=y)

        return dataset
