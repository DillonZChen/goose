import argparse
import json
import logging
import os
import pickle
from dataclasses import dataclass
from typing import Optional

import pymimir
from tqdm import tqdm

from learning.dataset import get_domain_file_from_opts, get_training_dir_from_opts
from planning.solution import get_plan
from util.paths import DATA_CACHE_DIR
from util.statistics import log_quartiles
from wlplan.planning import Action, Atom, Problem, State, parse_domain, parse_problem


@dataclass
class LabelledSuccessorData:
    action: Action
    successor_state: State
    value: Optional[int]


@dataclass
class LabelledStateAndSuccessorsData:
    state: State
    value: int
    successors_labelled: list[LabelledSuccessorData]

    def __repr__(self):
        return json.dumps(
            {
                "s": str(self.state),
                "value": self.value,
                "successors": [
                    {
                        "a": str(ss.action),
                        "s'": str(ss.successor_state),
                        "v": ss.value,
                    }
                    for ss in self.successors_labelled
                ],
            },
            indent=2,
        )


@dataclass
class LabelledProblemData:
    problem: Problem
    states_and_successors_labelled: list[LabelledStateAndSuccessorsData]


LabelledDataset = list[LabelledProblemData]


def log_dataset_statistics(dataset: LabelledDataset) -> None:
    v_values = []
    q_values = []
    for data in dataset:
        for states_and_successors in data.states_and_successors_labelled:
            v_values.append(states_and_successors.value)
            for successor in states_and_successors.successors_labelled:
                if successor.value is not None:
                    v_values.append(successor.value)
                    q_values.append(successor.value)
    log_quartiles(v_values, "V-value statistics")
    log_quartiles(q_values, "Q-value statistics")


class DatasetLabeller:
    def __init__(self, opts: argparse.Namespace):
        self._domain_path = get_domain_file_from_opts(opts)
        self._training_dir = get_training_dir_from_opts(opts)

        problems = sorted([f"{self._training_dir}/{f}" for f in os.listdir(self._training_dir) if f.endswith(".pddl")])
        problems = problems[: opts.num_data] if opts.num_data is not None else problems
        self._problem_paths = problems

        self._cache = opts.cache
        cache_file = os.path.normpath(opts.domain_directory).replace("/", "_").replace(".", "_")
        cache_file += f"_numdata_{opts.num_data}"
        self._cache_file = f"{DATA_CACHE_DIR}/{cache_file}.pkl"
        self._cache_file_exists = os.path.exists(self._cache_file)

    def get_labelled_dataset(self) -> LabelledDataset:
        if self._cache and self._cache_file_exists:
            with open(self._cache_file, "rb") as f:
                ret = pickle.load(f)
        else:
            self._domain = parse_domain(self._domain_path)
            self._name_to_predicate = {p.name: p for p in self._domain.predicates}
            self._name_to_schema = {s.name: s for s in self._domain.schemata}

            self._mimir_domain = pymimir.DomainParser(self._domain_path).parse()
            self._mimir_name_to_schema = {s.name: s for s in self._mimir_domain.action_schemas}

            ret = []

            for problem_path in tqdm(self._problem_paths):
                plan = get_plan(self._domain_path, problem_path)
                if plan is None:
                    continue
                problem = parse_problem(self._domain_path, problem_path, keep_statics=True)
                states_and_successors_labelled = self._compute_plan_trace_and_successors(problem_path, plan)

                ret.append(LabelledProblemData(problem, states_and_successors_labelled))

        if self._cache and not self._cache_file_exists:
            os.makedirs(os.path.dirname(self._cache_file), exist_ok=True)
            with open(self._cache_file, "wb") as f:
                pickle.dump(ret, f)

        return ret

    def _mimir_to_wlplan_state(self, mimir_state: pymimir.State) -> State:
        # repeated in classic_dataset_creator.py
        atoms = []
        for atom in mimir_state.get_atoms():
            predicate_name = atom.predicate.name
            if predicate_name == "=":
                continue
            wlplan_atom = Atom(
                predicate=self._name_to_predicate[predicate_name],
                objects=[o.name for o in atom.terms],
            )
            atoms.append(wlplan_atom)
        return State(atoms)

    def _mimir_to_wlplan_action(self, action: pymimir.Action) -> Action:
        schema = self._name_to_schema[action.schema.name]
        args = [o.name for o in action.get_arguments()]
        return Action(schema, args)

    def _string_to_mimir_action(self, mimir_problem: pymimir.Problem, action_str: str) -> pymimir.Action:
        name_to_object = {o.name: o for o in mimir_problem.objects}
        toks = action_str[1:-1].split()
        schema = self._mimir_name_to_schema[toks[0]]
        args = [name_to_object[arg] for arg in toks[1:]]
        return pymimir.Action.new(mimir_problem, schema, args)

    def _get_value_from_state(self, problem_template: str, state: State) -> Optional[int]:
        state_str = " ".join(atom.to_pddl() for atom in state.atoms)
        problem_template = problem_template.replace("<REPLACE>", state_str)

        new_problem_path = ".tmp/problem_from_state.pddl"
        os.makedirs(os.path.dirname(new_problem_path), exist_ok=True)
        with open(new_problem_path, "w") as f:
            f.write(problem_template)
        plan = get_plan(self._domain_path, new_problem_path)

        if plan is None:
            return None
        else:
            return len(plan)

    def _compute_plan_trace_and_successors(
        self, problem_path: str, plan: list[str]
    ) -> list[LabelledStateAndSuccessorsData]:
        states_and_successors_labelled = []

        # Problem template
        problem_template = open(problem_path, "r").read()
        init_content = problem_template[problem_template.find("(:init") + 6 :]
        stack = 0
        for i in range(len(init_content)):
            if init_content[i] == "(":
                stack += 1
            elif init_content[i] == ")":
                stack -= 1
            if stack < 0:
                init_content = init_content[:i]
                break
        problem_template = problem_template.replace(init_content, " <REPLACE> ")

        # Mimir stuff
        mimir_problem = pymimir.ProblemParser(problem_path).parse(self._mimir_domain)
        successor_generator = pymimir.LiftedSuccessorGenerator(mimir_problem)
        mimir_state = mimir_problem.create_state(mimir_problem.initial)

        # Collect states, successors, and labels
        for i, action_str in enumerate(plan):
            plan_action = self._string_to_mimir_action(mimir_problem, action_str)

            state = self._mimir_to_wlplan_state(mimir_state)
            value = len(plan) - i  # assume unit action costs
            successors = []
            for mimir_action in successor_generator.get_applicable_actions(mimir_state):
                action = self._mimir_to_wlplan_action(mimir_action)
                succ_state = self._mimir_to_wlplan_state(mimir_action.apply(mimir_state))
                succ_value = self._get_value_from_state(problem_template, succ_state)
                assert succ_value is None or succ_value >= value - 1, (succ_value, value)  # assume unit action costs

                successor_labelled = LabelledSuccessorData(action=action, successor_state=succ_state, value=succ_value)
                successors.append(successor_labelled)

            state_and_successors_labelled = LabelledStateAndSuccessorsData(
                state=state, value=value, successors_labelled=successors
            )
            states_and_successors_labelled.append(state_and_successors_labelled)

            mimir_state = plan_action.apply(mimir_state)

        return states_and_successors_labelled
