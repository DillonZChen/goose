import logging
import os
from abc import abstractmethod
from argparse import Namespace

import pymimir
import toml
from tqdm import tqdm

import wlplan
import wlplan.feature_generation
from learning.dataset.base_dataset import Dataset
from learning.dataset.cost_to_go_dataset import CostToGoDataset
from learning.dataset.ranking_dataset import RankingDataset, RankingGroup
from planning.core import get_downward_translation_atoms
from util.error_message import get_path_error_msg
from wlplan.data import Dataset as WLPlanDataset
from wlplan.data import ProblemStates
from wlplan.feature_generation import WLFeatures
from wlplan.planning import Predicate

DOMAINS = {
    "blocksworld",
    "childsnack",
    "ferry",
    "floortile",
    "miconic",
    "rovers",
    "satellite",
    "sokoban",
    "spanner",
    "transport",
}

MAX_EXPANSIONS = 100000


class _DatasetCreator:
    def __init__(
        self,
        data_config: str,
        feature_generator: WLFeatures,
        facts: str,
        max_data: int,
        hash_prefix: str,
    ):
        # domain information
        data_config = toml.load(data_config)

        self.domain_pddl = data_config["domain_pddl"]
        self.tasks_dir = data_config["tasks_dir"]
        self.plans_dir = data_config["plans_dir"]

        assert os.path.exists(self.domain_pddl), get_path_error_msg(self.domain_pddl)
        assert os.path.exists(self.tasks_dir), get_path_error_msg(self.tasks_dir)
        assert os.path.exists(self.plans_dir), get_path_error_msg(self.plans_dir)

        self.mimir_domain = pymimir.DomainParser(str(self.domain_pddl)).parse()
        self.wlplan_domain = wlplan.planning.parse_domain(self.domain_pddl)

        self.name_to_predicate = self._get_predicates(keep_statics=(facts != "nostatic"))
        predicates = sorted(list(self.name_to_predicate.values()), key=lambda x: repr(x))
        predicates = repr([repr(x) for x in predicates]).replace("'", "")
        logging.info(f"{facts=}")
        logging.info(f"{predicates=}")

        # feature generator
        self.feature_generator = feature_generator

        # facts in a state to keep
        self.atoms_to_keep = None
        self.facts = facts
        if facts == "fd":
            self.keep_atom_f = lambda atom: atom.get_name() in self.atoms_to_keep
        elif facts == "all":
            self.keep_atom_f = lambda _: True
        elif facts == "nostatic":
            self.keep_atom_f = lambda atom: atom.predicate.name in self.name_to_predicate
        else:
            raise ValueError(f"Unknown facts option {facts}")
        
        # max number of data
        self.max_data = max_data

        # prevent tmp files from being overwritten by parallel jobs
        self.hash_prefix = hash_prefix

    def _update_atoms_to_keep(self, problem_pddl: str):
        if self.facts == "fd":
            self.atoms_to_keep = get_downward_translation_atoms(
                self.domain_pddl,
                problem_pddl,
                hash_prefix=self.hash_prefix,
            )
        else:
            self.atoms_to_keep = None

    def _mimir_to_wlplan_state(self, mimir_state: pymimir.State) -> wlplan.planning.State:
        wlplan_state = []
        for atom in mimir_state.get_atoms():
            if not self.keep_atom_f(atom):
                continue
            wlplan_atom = wlplan.planning.Atom(
                predicate=self.name_to_predicate[atom.predicate.name],
                objects=[o.name for o in atom.terms],
            )
            wlplan_state.append(wlplan_atom)
        return wlplan_state

    def _get_problem_iterator(self, plans_only=True):
        pbar = []
        if not plans_only:
            pbar = [self.tasks_dir + "/" + f for f in sorted(os.listdir(self.tasks_dir))]
        else:
            for f in sorted(os.listdir(self.plans_dir)):
                problem_pddl = self.tasks_dir + "/" + f.replace(".plan", ".pddl")
                plan_file = self.plans_dir + "/" + f
                pbar.append((problem_pddl, plan_file))
        pbar = tqdm(pbar, desc="Collecting data from problems")
        return pbar

    def _get_predicates(self, keep_statics: bool) -> dict[str, Predicate]:
        predicates = {}
        if keep_statics:
            for predicate in self.mimir_domain.predicates:
                name = predicate.name
                arity = predicate.arity
                predicates[name] = Predicate(name=name, arity=arity)
        else:
            for schema in self.mimir_domain.action_schemas:
                for effect in schema.effect:
                    atom = effect.atom
                    predicate = atom.predicate
                    name = predicate.name
                    arity = predicate.arity
                    predicate = Predicate(name=name, arity=arity)
                    if name not in predicates:
                        predicates[name] = predicate
                    else:
                        assert predicates[name] == predicate
        return predicates

    def _collect_actions_from_plan(self, mimir_problem: pymimir.Problem, plan_file: str) -> list[pymimir.Action]:
        name_to_schema = {s.name: s for s in self.mimir_domain.action_schemas}
        name_to_object = {o.name: o for o in mimir_problem.objects}
        actions = []
        with open(plan_file, "r") as f:
            lines = f.readlines()
            for line in lines:
                if line.startswith(";"):
                    continue
                action_name = line.strip()
                action_name = action_name.replace("(", "")
                action_name = action_name.replace(")", "")
                toks = action_name.split(" ")
                schema = toks[0]
                schema = name_to_schema[schema]
                args = toks[1:]
                args = [name_to_object[arg] for arg in args]
                action = pymimir.Action.new(mimir_problem, schema, args)
                actions.append(action)
        return actions

    @abstractmethod
    def get_dataset(self) -> Dataset:
        pass


class _StateSpaceDatasetCreator(_DatasetCreator):
    def __init__(self, max_expanded: int = MAX_EXPANSIONS, **kwargs):
        super().__init__(**kwargs)
        self.max_expanded = max_expanded


class _CostToGoDatasetFromPlans(_DatasetCreator):
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
                wlplan_states.append(self._mimir_to_wlplan_state(mimir_state))
                y.append(h_opt)

            wlplan_data.append((wlplan_problem, wlplan_states))

        data = []
        for problem, states in wlplan_data:
            data.append(ProblemStates(problem=problem, states=states))
        dataset = CostToGoDataset(wlplan_domain=self.wlplan_domain, data=data, y=y)

        return dataset


class _CostToGoDatasetFromStateSpace(_StateSpaceDatasetCreator):
    def get_dataset(self) -> CostToGoDataset:
        wlplan_data = []
        y = []
        seen_x_y_pairs = set()

        for problem_pddl in self._get_problem_iterator(plans_only=False):
            if len(seen_x_y_pairs) >= self.max_data:
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
                if len(seen_x_y_pairs) >= self.max_data:
                    break
                h = ss.get_distance_to_goal_state(state)
                wlplan_state = self._mimir_to_wlplan_state(state)

                # check if WL repr of the state has been seen before
                mini_dataset = WLPlanDataset(
                    domain=self.wlplan_domain, data=[ProblemStates(problem=wlplan_problem, states=[wlplan_state])]
                )
                self.feature_generator.collect(mini_dataset)
                x_repr = self.feature_generator.get_string_representation(wlplan_state)
                if (x_repr, h) in seen_x_y_pairs:
                    continue

                seen_x_y_pairs.add((x_repr, h))
                wlplan_states.append(wlplan_state)
                y.append(h)

            wlplan_data.append((wlplan_problem, wlplan_states))
            if len(seen_x_y_pairs) >= self.max_data:
                break

        data = []
        for problem, states in wlplan_data:
            data.append(ProblemStates(problem=problem, states=states))
        dataset = CostToGoDataset(wlplan_domain=self.wlplan_domain, data=data, y=y)

        return dataset


class _RankingDatasetFromPlans(_DatasetCreator):
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
            data.append(ProblemStates(problem=problem, states=states))
        dataset = RankingDataset(wlplan_domain=self.wlplan_domain, data=data, y=y)

        return dataset


def get_dataset(opts: Namespace, feature_generator: WLFeatures) -> Dataset:
    """State space datasets automatically remove WL-indistinguishable states with equivalent target values."""

    rank = opts.rank
    data_generation = opts.data_generation

    kwargs = {
        "feature_generator": feature_generator,
        "data_config": opts.data_config,
        "facts": opts.facts,
        "max_data": opts.max_data,
        "hash_prefix": str(hash(repr(opts))),
    }

    rank_err_msg = "Ranking dataset from state space not supported as it will be too large and constraining."

    match (rank, data_generation):
        case (False, "plan"):
            return _CostToGoDatasetFromPlans(**kwargs).get_dataset()
        case (False, "state-space"):
            return _CostToGoDatasetFromStateSpace(**kwargs).get_dataset()
        case (False, "all"):
            dataset_1 = _CostToGoDatasetFromPlans(**kwargs).get_dataset()
            dataset_2 = _CostToGoDatasetFromStateSpace(**kwargs).get_dataset()
            domain = dataset_1.domain
            data = dataset_1.data + dataset_2.data
            y = dataset_1.y + dataset_2.y
            dataset = CostToGoDataset(wlplan_domain=domain, data=data, y=y)
            return dataset
        case (True, "plan"):
            return _RankingDatasetFromPlans(**kwargs).get_dataset()
        case (True, "state-space"):
            raise ValueError(rank_err_msg)
        case (True, "all"):
            raise ValueError(rank_err_msg)
        case _:
            raise ValueError(f"Unknown data_generation {data_generation}")
