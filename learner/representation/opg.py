""" Object-Pair Binary Structure """
import itertools
from enum import Enum

import torch
from torch import Tensor

from learner.representation.base_class import (CGraph, LiftedState,
                                               Representation, TGraph)
from learner.representation.planning.translate.pddl import Atom, NegatedAtom
from learner.util import pair_to_index_map


class WlColours(Enum):
    F_POS_GOAL = 0
    T_POS_GOAL = 1
    T_NON_GOAL = 2


_F = len(WlColours)


class ObjectPairGraph(Representation):
    name = "opg"
    lifted = True

    def __init__(self, domain_pddl: str, problem_pddl: str):
        self._get_problem_info(domain_pddl, problem_pddl)
        self.colour_explanation = {
            0: "so",  # single object
            1: "po",  # pair object
        }
        self.offset = len(self.colour_explanation)
        for i, pred in enumerate(self.predicates):
            self.colour_explanation[
                self.offset + _F * i + WlColours.F_POS_GOAL.value
            ] = f"ug {pred.name}"
            self.colour_explanation[
                self.offset + _F * i + WlColours.T_POS_GOAL.value
            ] = f"ag {pred.name}"
            self.colour_explanation[
                self.offset + _F * i + WlColours.T_NON_GOAL.value
            ] = f"ap {pred.name}"

        n = self.largest_predicate_size
        super().__init__(
            domain_pddl,
            problem_pddl,
            n_node_features=len(self.colour_explanation),
            n_edge_labels=2 + ((n * (n + 1)) // 2),
        )

    def str_to_state(self, s) -> LiftedState:
        """Used in dataset construction to convert string representation of facts into a (pred, [args]) representation"""
        state = []
        for fact in s:
            fact = fact.replace(")", "").replace("(", "")
            toks = fact.split()
            if toks[0] == "=":
                continue
            if len(toks) > 1:
                state.append((toks[0], toks[1:]))
            else:
                state.append((toks[0], ()))
        return state

    def state_to_tgraph(self, state: LiftedState) -> TGraph:
        raise NotImplementedError

    def state_to_cgraph(self, state: LiftedState) -> CGraph:
        G = self.G.copy()
        new_idx = len(self._node_to_i)

        for fact in state:
            pred = fact[0]
            objs = fact[1]

            colour_start = self.offset + _F * self.pred_to_idx[pred]

            if len(pred) == 0:
                continue

            node = (pred, tuple(objs))

            # activated proposition overlaps with a goal Atom
            if node in self._pos_goal_nodes:
                col = colour_start + WlColours.T_POS_GOAL.value
                G.nodes[node]["x"] = col
                continue

            node = new_idx
            new_idx += 1

            # else add node and corresponding edges to graph
            col = colour_start + WlColours.T_NON_GOAL.value
            G.add_node(node, x=col)

            obj_to_index = {}
            for k, obj in enumerate(objs):
                obj_to_index[obj] = k
                # connect fact to object
                assert obj in G.nodes, obj
                G.add_edge(u_of_edge=node, v_of_edge=obj, edge_label=k)
                G.add_edge(v_of_edge=node, u_of_edge=obj, edge_label=k)

            n = self.largest_predicate_size
            for obj_pair in itertools.combinations(objs, 2):
                obj_pair = tuple(sorted(obj_pair))
                assert obj_pair in G.nodes, obj_pair
                obj1, obj2 = obj_pair
                k1 = obj_to_index[obj1]
                k2 = obj_to_index[obj2]
                k = n + pair_to_index_map(n, k1, k2)
                G.add_edge(u_of_edge=node, v_of_edge=obj_pair, edge_label=k)
                G.add_edge(v_of_edge=node, u_of_edge=obj_pair, edge_label=k)

        return G

    def _compute_graph_representation(self) -> None:
        G = self._init_graph()

        # objects
        obj_names = [obj.name for obj in self.problem.objects]
        for obj in obj_names:
            G.add_node(obj, x=0)  # add object node
        for obj_pair in itertools.combinations(obj_names, 2):
            obj_pair = tuple(sorted(obj_pair))
            G.add_node(obj_pair, x=1)  # add object pair node
            obj1, obj2 = obj_pair
            for u, dec in [(obj1, 1), (obj2, 2)]:
                label = self.n_edge_labels - dec
                G.add_edge(u_of_edge=u, v_of_edge=obj_pair, edge_label=label)
                G.add_edge(v_of_edge=u, u_of_edge=obj_pair, edge_label=label)

        # goal (state gets dealt with in state_to_tgraph)
        if len(self.problem.goal.parts) == 0:
            goals = [self.problem.goal]
        else:
            goals = self.problem.goal.parts
        for fact in sorted(goals):
            assert type(fact) in {Atom, NegatedAtom}

            # may have negative goals
            if type(fact) == NegatedAtom:
                raise NotImplementedError

            pred = fact.predicate
            objs = fact.args
            goal = (pred, objs)

            col = (
                self.offset
                + _F * self.pred_to_idx[pred]
                + WlColours.F_POS_GOAL.value
            )
            G.add_node(goal, x=col)  # add fact node

            self._pos_goal_nodes.add(goal)

            obj_to_index = {}
            for k, obj in enumerate(objs):
                obj_to_index[obj] = k
                # connect fact to object
                G.add_edge(u_of_edge=goal, v_of_edge=obj, edge_label=k)
                G.add_edge(v_of_edge=goal, u_of_edge=obj, edge_label=k)

            n = self.largest_predicate_size
            for obj_pair in itertools.combinations(objs, 2):
                obj_pair = tuple(sorted(obj_pair))
                obj1, obj2 = obj_pair
                k1 = obj_to_index[obj1]
                k2 = obj_to_index[obj2]
                k = n + pair_to_index_map(n, k1, k2)
                G.add_edge(u_of_edge=goal, v_of_edge=obj_pair, edge_label=k)
                G.add_edge(v_of_edge=goal, u_of_edge=obj_pair, edge_label=k)
        # end goal

        # map node name to index
        self._node_to_i = {}
        for i, node in enumerate(G.nodes):
            self._node_to_i[node] = i
        self.G = G

        return

    def _colour_to_tensor(self, colour: int) -> Tensor:
        return self._one_hot_node(colour)
