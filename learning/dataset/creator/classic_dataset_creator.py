import argparse
import logging
from abc import abstractmethod

import pymimir

import wlplan
from learning.dataset.container.base_dataset import Dataset
from learning.dataset.creator.dataset_creator import DatasetCreator
from planning.util import get_downward_translation_atoms
from wlplan.planning import Predicate, State


class ClassicDatasetCreator(DatasetCreator):
    """Base class for creating datasets for classical planning. Relies on the mimir package."""

    def __init__(self, opts: argparse.Namespace):
        super().__init__(opts)

        facts = opts.facts

        self.mimir_domain = pymimir.DomainParser(str(self.domain_pddl)).parse()
        self.name_to_predicate = self._get_predicates(keep_statics=(facts != "nostatic"))
        predicates = sorted(list(self.name_to_predicate.values()), key=lambda x: repr(x))
        predicates = repr([repr(x) for x in predicates]).replace("'", "")
        logging.info(f"{facts=}")
        logging.info(f"{predicates=}")

        # facts in a state to keep
        self.atoms_to_keep = None
        self.facts = facts
        if self.facts == "fd":
            self.keep_atom_f = lambda atom: atom.get_name() in self.atoms_to_keep
        elif self.facts == "all":
            self.keep_atom_f = lambda _: True
        elif self.facts == "nostatic":
            self.keep_atom_f = lambda atom: atom.predicate.name in self.name_to_predicate
        else:
            raise ValueError(f"Unknown facts option {self.facts}")

    def _update_atoms_to_keep(self, problem_pddl: str):
        if self.facts == "fd":
            self.atoms_to_keep = get_downward_translation_atoms(
                self.domain_pddl,
                problem_pddl,
                hash_prefix=self._hash_prefix,
            )
        else:
            self.atoms_to_keep = None

    def _mimir_to_wlplan_state(self, mimir_state: pymimir.State) -> wlplan.planning.State:
        atoms = []
        for atom in mimir_state.get_atoms():
            if not self.keep_atom_f(atom):
                continue
            predicate_name = atom.predicate.name
            if predicate_name == "=":
                continue
            wlplan_atom = wlplan.planning.Atom(
                predicate=self.name_to_predicate[predicate_name],
                objects=[o.name for o in atom.terms],
            )
            atoms.append(wlplan_atom)
        return State(atoms)

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
