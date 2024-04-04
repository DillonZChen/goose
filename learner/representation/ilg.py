""" ILG from ICAPS-24 submission. Differs from GenPlan-23 submission. """
import torch
from enum import Enum
from torch import Tensor
from .planning.translate.pddl import Atom, NegatedAtom
from .base_class import Representation, LiftedState, TGraph, CGraph


class WlColours(Enum):
    F_POS_GOAL = 0
    T_POS_GOAL = 1
    T_NON_GOAL = 2


_F = len(WlColours)


class InstanceLearningGraph(Representation):
    name = "ilg"
    lifted = True

    def __init__(self, domain_pddl: str, problem_pddl: str):
        self._get_problem_info(domain_pddl, problem_pddl)
        super().__init__(
            domain_pddl,
            problem_pddl,
            n_node_features=1 + self.n_predicates * _F,
            n_edge_labels=self.largest_predicate_size,
        )

        self.colour_explanation = {
            0: "ob",  # object
        }
        for i, pred in enumerate(self.predicates):
            self.colour_explanation[
                1 + _F * i + WlColours.F_POS_GOAL.value
            ] = f"ug {pred.name}"
            self.colour_explanation[
                1 + _F * i + WlColours.T_POS_GOAL.value
            ] = f"ag {pred.name}"
            self.colour_explanation[
                1 + _F * i + WlColours.T_NON_GOAL.value
            ] = f"ap {pred.name}"

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
        """States are represented as a list of (pred, [args])"""
        x = self.x.clone()
        edge_indices = self.edge_indices.copy()
        i = len(x)

        to_add = sum(len(fact[1]) + 1 for fact in state)
        x = torch.nn.functional.pad(x, (0, 0, 0, to_add), "constant", 0)
        new_edges = {i: [] for i in range(-1, self.largest_predicate_size)}

        for fact in state:
            pred = fact[0]
            args = fact[1]

            if len(pred) == 0:
                continue

            colour_start = 1 + _F * self.pred_to_idx[pred]

            if len(pred) == 0:
                continue

            node = (pred, tuple(args))

            # activated proposition overlaps with a goal
            if node in self._node_to_i:
                col = colour_start + WlColours.T_POS_GOAL.value
                x[self._node_to_i[node]][col] = 1
                continue

            # activated proposition does not overlap with a goal
            col = colour_start + WlColours.T_NON_GOAL.value
            x[i][col] = 1

            true_node_i = i
            i += 1

            # connect fact to objects
            for k, arg in enumerate(args):
                new_edges[k].append((true_node_i, self._node_to_i[arg]))
                new_edges[k].append((self._node_to_i[arg], true_node_i))

        for i, new_edges in new_edges.items():
            edge_indices[i] = torch.hstack(
                (edge_indices[i], torch.tensor(new_edges).T)
            ).long()

        return x, edge_indices

    def state_to_cgraph(self, state: LiftedState) -> CGraph:
        """States are represented as a list of (pred, [args])"""
        c_graph = self.G.copy()

        for fact in state:
            pred = fact[0]
            args = fact[1]

            colour_start = 1 + _F * self.pred_to_idx[pred]

            if len(pred) == 0:
                continue

            node = (pred, tuple(args))

            # activated proposition overlaps with a goal Atom
            if node in self._pos_goal_nodes:
                col = colour_start + WlColours.T_POS_GOAL.value
                c_graph.nodes[node]["x"] = col
                continue

            # else add node and corresponding edges to graph
            col = colour_start + WlColours.T_NON_GOAL.value
            c_graph.add_node(node, x=col)

            for k, obj in enumerate(args):
                # connect fact to object
                assert obj in c_graph.nodes
                c_graph.add_edge(u_of_edge=node, v_of_edge=obj, edge_label=k)
                c_graph.add_edge(v_of_edge=node, u_of_edge=obj, edge_label=k)

        return c_graph

    def _compute_graph_representation(self) -> None:
        G = self._init_graph()

        # objects
        for i, obj in enumerate(sorted(self.problem.objects)):
            G.add_node(obj.name, x=0)  # add object node

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
            args = fact.args
            goal_node = (pred, args)

            col = 1 + _F * self.pred_to_idx[pred] + WlColours.F_POS_GOAL.value
            G.add_node(goal_node, x=col)  # add fact node

            self._pos_goal_nodes.add(goal_node)

            for k, arg in enumerate(args):
                # connect fact to object
                G.add_edge(u_of_edge=goal_node, v_of_edge=arg, edge_label=k)
                G.add_edge(v_of_edge=goal_node, u_of_edge=arg, edge_label=k)
        # end goal

        # map node name to index
        self._node_to_i = {}
        for i, node in enumerate(G.nodes):
            self._node_to_i[node] = i
        self.G = G

        return

    def _colour_to_tensor(self, colour: int) -> Tensor:
        return self._one_hot_node(colour)
