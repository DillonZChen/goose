import argparse
import random

import torch
from succgen.planning.action import SGAction
from succgen.planning.state import SGState
from typing_extensions import override

from learning.predictor.neural_network.serialise import load_gnn
from learning.pyg import wlplan_graph_to_pyg
from planning.policy.policy import PolicyExecutor
from wlplan.graph_generator import init_graph_generator
from wlplan.planning import Action, Atom, State, to_wlplan_domain, to_wlplan_problem


class GnnPolicyExecutor(PolicyExecutor):
    def __init__(
        self,
        domain_file: str,
        problem_file: str,
        gnn: torch.nn.Module,
        train_opts: argparse.Namespace,
        debug: bool = False,
        bound: int = -1,
    ):
        super().__init__(domain_file=domain_file, problem_file=problem_file, debug=debug, bound=bound)

        # Set seeds
        self._device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        self._gnn = gnn.to(self._device)
        self._policy_type = train_opts.policy_type
        match self._policy_type:
            case "v":
                self._select_fn = self._select_v
            case "q":
                self._select_fn = self._select_q
            case "a":
                self._select_fn = self._select_a
            case "d":
                self._select_fn = self._select_d
            case _:
                raise ValueError(f"Unknown value {self._policy_type=}")

        # WLPlan components
        self._domain = to_wlplan_domain(self._task.domain)
        self._problem = to_wlplan_problem(self._task.domain, self._task.problem)

        self._name_to_predicate = {p.name: p for p in self._domain.predicates}
        self._name_to_schema = {s.name: s for s in self._domain.schemata}

        self._graph_generator = init_graph_generator(
            graph_representation=train_opts.graph_representation,
            domain=self._domain,
            differentiate_constant_objects=True,
        )
        self._graph_generator.set_problem(self._problem)

    def _sgstate_to_wlstate(self, state: SGState) -> State:
        wl_state = []
        for atom in state.atoms:
            atom = self._task.atom_packer.unpack(atom)
            predicate = self._name_to_predicate[self._task.i_to_pred[atom[0]]]
            objects = [self._task.i_to_obj[i] for i in atom[1]]
            atom = Atom(predicate=predicate, objects=objects)
            wl_state.append(atom)
        wl_state = State(atoms=wl_state)
        return wl_state

    def _sgaction_to_wlaction(self, action: SGAction) -> Action:
        schema = self._name_to_schema[self._task.i_to_schema[action[0]]]
        objects = [self._task.i_to_obj[i] for i in action[1]]
        wl_action = Action(schema=schema, objects=objects)
        return wl_action

    def _select_v(self, state: SGState, actions: list[SGAction]) -> SGAction:
        raise NotImplementedError

    def _select_q(self, state: SGState, actions: list[SGAction]) -> SGAction:
        best_pred = float("inf")
        best_action = None
        random.shuffle(actions)
        for action in actions:
            succ = self._get_successor_state(action, state)
            wl_state = self._sgstate_to_wlstate(succ)
            wl_action = self._sgaction_to_wlaction(action)
            graph = self._graph_generator.to_graph(state=wl_state, actions=[wl_action])
            pyg = wlplan_graph_to_pyg(self._graph_generator, graph).to(self._device)
            pred = self._gnn.forward(x=pyg.x, edge_indices_list=pyg.edge_index)
            pred = pred.squeeze().item()  # Assuming pred is a single value tensor
            if pred < best_pred:
                best_pred = pred
                best_action = action
        return best_action

    def _select_a(self, state: SGState, actions: list[SGAction]) -> SGAction:
        return self._select_q(state, actions)

    def _select_d(self, state: SGState, actions: list[SGAction]) -> SGAction:
        raise NotImplementedError

    @override
    def select_action(self, state: SGState, actions: list[SGAction]) -> SGAction:
        return self._select_fn(state, actions)
