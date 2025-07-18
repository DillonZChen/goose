import pymimir

import wlplan
from goose.learning.dataset.heuristic.container.ranking_dataset import RankingDataset, RankingGroup
from goose.learning.dataset.heuristic.creator.classic_dataset_creator import ClassicDatasetCreator
from wlplan.data import ProblemDataset


class ClassicRankingDatasetFromPlans(ClassicDatasetCreator):
    def get_dataset(self) -> RankingDataset:
        wlplan_data = []
        y: list[RankingGroup] = []
        states_added = 0

        for problem_pddl, plan_file in self._get_problem_iterator():
            self._update_atoms_to_keep(problem_pddl)

            # parse problem with mimir
            mimir_problem = pymimir.ProblemParser(str(problem_pddl)).parse(self.mimir_domain)
            mimir_state = mimir_problem.create_state(mimir_problem.initial)
            successor_generator = pymimir.LiftedSuccessorGenerator(mimir_problem)

            wlplan_problem = wlplan.planning.parse_problem(self.domain_pddl, problem_pddl)

            # collect actions
            opt_actions = self._collect_actions_from_plan(mimir_problem, plan_file)
            opt_action_names = [a.get_name() for a in opt_actions]

            # collect plan trace states
            wlplan_states = []
            s_index = 0

            def _deal_with_state(mimir_state: pymimir.State):
                # there may be some redundantly added states e.g. same successor of 2 different states
                # the gain from optimising this is probably quite marginal and takes a bit of effort
                nonlocal opt_actions
                nonlocal states_added
                nonlocal s_index

                good_group = []
                maybe_group = []
                bad_group = []

                # the parent state is bad as it is definitely worse than the successor
                bad_group.append(states_added)
                wlplan_states.append(self._mimir_to_wlplan_state(mimir_state))
                states_added += 1

                # look at the successor states
                succ_actions = successor_generator.get_applicable_actions(mimir_state)
                for action in succ_actions:
                    succ_state = action.apply(mimir_state)
                    if action.get_name() == opt_action_names[s_index]:
                        # the succ state from the optimal action is definitely good
                        good_group.append(states_added)
                        wlplan_states.append(self._mimir_to_wlplan_state(succ_state))
                        states_added += 1
                    else:
                        # we do not know if the siblings are good or bad
                        maybe_group.append(states_added)
                        wlplan_states.append(self._mimir_to_wlplan_state(succ_state))
                        states_added += 1

                y.append(RankingGroup(good_group, maybe_group, bad_group))
                s_index += 1

            _deal_with_state(mimir_state)
            for action in opt_actions:
                mimir_state = action.apply(mimir_state)
                _deal_with_state(mimir_state)
                if s_index >= len(opt_actions):
                    # skip ranking of goal state
                    break

            wlplan_data.append((wlplan_problem, wlplan_states))

        data = []
        for problem, states in wlplan_data:
            data.append(ProblemDataset(problem=problem, states=states))
        dataset = RankingDataset(wlplan_domain=self._wlplan_domain, data=data, y=y)

        return dataset
