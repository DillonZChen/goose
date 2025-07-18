import argparse
from abc import abstractmethod

import wlplan
import wlplan.planning
from goose.enums.state_representation import StateRepresentation
from goose.learning.dataset.heuristic.container.base_dataset import Dataset
from goose.learning.dataset.heuristic.creator.dataset_creator import DatasetCreator
from goose.planning.util import call_numeric_downward
from wlplan.planning import Atom, State


class NumericDatasetCreator(DatasetCreator):
    """Base class for creating datasets for numeric planning. Relies on Numeric Fast Downward."""

    def __init__(self, opts: argparse.Namespace):
        super().__init__(opts)
        NFD = StateRepresentation.NUMERIC_DOWNWARD
        if opts.state_representation != NFD:
            raise ValueError(f"Numeric configs must use Numeric Downward, so facts must be '{NFD}'")

        self.name_to_predicate = {p.name: p for p in self._wlplan_domain.predicates}
        self.name_to_function = {f.name: f for f in self._wlplan_domain.functions}

    def _nfd_to_wlplan_state(self, input: str, problem_info) -> State:
        if len(input) == 0:
            return None
        toks = input.split(";")
        fluent_name_to_id = problem_info["fluent_name_to_id"]

        # atoms
        atoms = []
        true_facts = sorted(toks[0].split("?"))
        true_facts = set([f for f in true_facts if len(f) > 0])
        for f in true_facts:
            predicate = self.name_to_predicate[f.split("(")[0]]
            objects = f.split("(")[1][:-1].split(", ")
            objects = [o for o in objects if len(o) > 0]
            # assert all(o in problem_info["objects"] for o in objects)
            for o in objects:
                if o not in problem_info["objects"]:
                    print(f"Error: {o} not in {problem_info['objects']=}")
                    print(f"{input=}")
                    print(f"{f=}")
                    exit(-1)
            atom = Atom(predicate, objects)
            atoms.append(atom)

        # fluents
        fluents = []
        for tok in toks[1].split("?"):
            if len(tok) == 0:
                continue
            toks = tok.split(":")
            var = toks[0]
            val = float(toks[1])
            fluents.append((var, val))
        values = [0.0] * len(fluent_name_to_id)
        for var, val in fluents:
            values[fluent_name_to_id[var]] = val

        state = State(atoms, values)
        return state

    def _get_nfd_info(self, problem_pddl: str, plan_file: str):
        problem = wlplan.planning.parse_problem(self.domain_pddl, problem_pddl)
        fluents = problem.fluents
        fluent_name_to_id = {str(f): i for i, f in enumerate(fluents)}
        problem_info = {
            "fluent_name_to_id": fluent_name_to_id,
            "objects": set(problem.objects).union(set(problem.constant_objects)),
        }

        config = f"plan_trace_successors(plan_path={plan_file})"
        output = call_numeric_downward(
            self.domain_pddl,
            problem_pddl,
            config=config,
            hash_prefix=self._hash_prefix,
        )

        output = output[output.find("__START_HERE__") + len("__START_HERE__") :]
        output = output[: output.find("__END_HERE__")]
        lines = output.split("\n")

        action_strings = []  # computed but not used currently
        ranking_groups = []

        nfd_fact_names = []  # computed but not used currently
        nfd_fluent_names = []

        for line in lines:
            if line.startswith("_atoms"):
                nfd_fact_names = line.split("|")[1:]
                nfd_fact_names = [f for f in nfd_fact_names if len(f) > 0]
            elif line.startswith("_fluents"):
                nfd_fluent_names = line.split("|")[1:]
                nfd_fluent_names = [f for f in nfd_fluent_names if len(f) > 0]
            elif line.startswith("_action"):
                toks = line.split("^")
                action = toks[1].replace(" ", "_")
                action_strings.append(action)
            elif line.startswith("_state"):
                toks = line.split("|")
                parent_string = toks[1]
                optimal_string = toks[2]
                siblings_strings = toks[3:]

                parent = self._nfd_to_wlplan_state(parent_string, problem_info)
                optimal = self._nfd_to_wlplan_state(optimal_string, problem_info)
                siblings = [self._nfd_to_wlplan_state(s, problem_info) for s in siblings_strings]

                ranking_group = {
                    "good_group": [optimal],
                    "maybe_group": siblings,
                    "bad_group": [parent],
                }
                ranking_groups.append(ranking_group)

        pddl_fluent_names = set(str(f) for f in problem.fluents)
        nfd_fluent_names = set(nfd_fluent_names)
        difference = list(pddl_fluent_names.difference(nfd_fluent_names))

        info = {
            "nfd_pruned_fluent_names": difference,
            "ranking_groups": ranking_groups,
        }

        return info

    @abstractmethod
    def get_dataset(self) -> Dataset:
        pass
