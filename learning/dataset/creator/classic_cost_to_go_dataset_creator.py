from typing import Optional

import pymimir

import wlplan
from learning.dataset.container.cost_to_go_dataset import CostToGoDataset
from learning.dataset.creator.classic_dataset_creator import ClassicDatasetCreator
from wlplan.data import Dataset as WLPlanDataset
from wlplan.data import ProblemStates

from .dataset_creator import MAX_EXPANSIONS_PER_PROBLEM, MAX_STATE_SPACE_DATA


class ClassicCostToGoDatasetFromStateSpace(ClassicDatasetCreator):
    def __init__(self, max_expanded: Optional[int] = MAX_EXPANSIONS_PER_PROBLEM, **kwargs):
        super().__init__(**kwargs)
        self.max_expanded = max_expanded

    def get_dataset(self) -> CostToGoDataset:
        wlplan_data = []
        y = []
        seen_x_y_pairs = set()

        for problem_pddl in self._get_problem_iterator(plans_only=False):
            if len(seen_x_y_pairs) >= MAX_STATE_SPACE_DATA:
                break

            self._update_atoms_to_keep(problem_pddl)

            # parse problem with mimir
            mimir_problem = pymimir.ProblemParser(str(problem_pddl)).parse(self.mimir_domain)
            wlplan_problem = wlplan.planning.parse_problem(self.domain_pddl, problem_pddl)

            succ_generator = pymimir.GroundedSuccessorGenerator(mimir_problem)
            ss = pymimir.StateSpace.new(mimir_problem, succ_generator, max_expanded=self.max_expanded)

            if ss is None:
                # reached max ss size, and assume train problems are monotonic in ss size
                break

            wlplan_states = []
            for state in ss.get_states():
                if len(seen_x_y_pairs) >= MAX_STATE_SPACE_DATA:
                    break
                h = ss.get_distance_to_goal_state(state)
                if h == -1:
                    continue
                wlplan_state = self._mimir_to_wlplan_state(state)

                # check if WL repr of the state has been seen before
                mini_dataset = WLPlanDataset(
                    domain=self._wlplan_domain,
                    data=[ProblemStates(problem=wlplan_problem, states=[wlplan_state])],
                )
                pruning = self._feature_generator.get_pruning()
                self._feature_generator.set_pruning("none")
                self._feature_generator.collect(mini_dataset)
                self._feature_generator.set_pruning(pruning)
                x_repr = self._feature_generator.get_string_representation(wlplan_state)
                if (x_repr, h) in seen_x_y_pairs:
                    continue

                seen_x_y_pairs.add((x_repr, h))
                wlplan_states.append(wlplan_state)
                y.append(h)

            wlplan_data.append((wlplan_problem, wlplan_states))
            if len(seen_x_y_pairs) >= MAX_STATE_SPACE_DATA:
                break

        data = []
        for problem, states in wlplan_data:
            data.append(ProblemStates(problem=problem, states=states))
        dataset = CostToGoDataset(wlplan_domain=self._wlplan_domain, data=data, y=y)

        return dataset


class ClassicCostToGoDatasetFromPlans(ClassicDatasetCreator):
    def get_dataset(self) -> CostToGoDataset:
        wlplan_data = []
        y = []

        for problem_pddl, plan_file in self._get_problem_iterator():
            self._update_atoms_to_keep(problem_pddl)

            # parse problem with mimir
            mimir_problem = pymimir.ProblemParser(str(problem_pddl)).parse(self.mimir_domain)
            mimir_state = mimir_problem.create_state(mimir_problem.initial)

            wlplan_problem = wlplan.planning.parse_problem(self.domain_pddl, problem_pddl)

            # collect actions
            opt_actions = self._collect_actions_from_plan(mimir_problem, plan_file)

            # collect plan trace states
            wlplan_states = []
            h_opt = len(opt_actions)
            wlplan_states.append(self._mimir_to_wlplan_state(mimir_state))
            y.append(h_opt)
            for action in opt_actions:
                h_opt -= 1
                # ICAPS-24 version did not include goal states
                if h_opt == 0:
                    break
                mimir_state = action.apply(mimir_state)
                wlplan_state = self._mimir_to_wlplan_state(mimir_state)
                wlplan_states.append(wlplan_state)
                y.append(h_opt)

            wlplan_data.append((wlplan_problem, wlplan_states))

        data = []
        for problem, states in wlplan_data:
            data.append(ProblemStates(problem=problem, states=states))
        dataset = CostToGoDataset(wlplan_domain=self._wlplan_domain, data=data, y=y)

        return dataset
