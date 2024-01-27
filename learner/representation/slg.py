""" SLG from AAAI-24 submission. """
from enum import Enum
from .planning.translate.pddl import Literal, Atom, NegatedAtom
from .planning.translate.instantiate import explore
from .base_class import (
    Representation,
    GroundedState,
    CGraph,
    TGraph,
    Tensor,
    AaaiAchievedWlColours,
)


class SlgFeatures(Enum):
    ACTION = 0
    POS_GOAL = 1
    NEG_GOAL = 2
    NON_GOAL = 3
    ACH_PROP = 4


class SlgEdgeLabels(Enum):
    PRE_EDGE = 0
    ADD_EDGE = 1
    DEL_EDGE = 2


class StripsLearningGraph(Representation):
    name = "slg"
    lifted = False

    def __init__(self, domain_pddl: str, problem_pddl: str):
        super().__init__(
            domain_pddl,
            problem_pddl,
            n_node_features=len(SlgFeatures),
            n_edge_labels=len(SlgEdgeLabels),
        )

    def str_to_state(self, state) -> GroundedState:
        return state

    def state_to_tgraph(self, state: GroundedState) -> TGraph:
        x = self.x.clone()
        for p in state:
            # assert p in self._node_to_i
            x[self._node_to_i[p]][SlgFeatures.ACH_PROP.value] = 1
        return x, self.edge_indices

    def state_to_cgraph(self, state: GroundedState) -> CGraph:
        c_graph = self.G.copy()
        for p in state:
            if p in self._pos_goal_nodes:
                colour = AaaiAchievedWlColours.ACH_POS_GOAL
            elif p in self._neg_goal_nodes:
                colour = AaaiAchievedWlColours.ACH_NEG_GOAL
            else:
                colour = AaaiAchievedWlColours.ACH_NON_GOAL
            c_graph.nodes[p]["x"] = colour.value
        return c_graph

    def _get_grounded_problem_info(self):
        """Ground the parsed lifted pddl representation
        and return propositions, actions, positive and negative goals.

        This can be potentially optimised by letting the planner send the grounded information here.
        """

        # Grounding the lifted representation.
        grounded = explore(self.problem)

        propositions = set(grounded[1])
        actions = grounded[2]

        goals = (
            self.problem.goal.parts
            if len(self.problem.goal.parts) > 0
            else [self.problem.goal]
        )
        positive_goals = set()
        negative_goals = set()
        for goal in goals:
            if type(goal) == Atom:
                positive_goals.add(goal)
            elif type(goal) == NegatedAtom:
                negative_goals.add(goal)
            else:
                raise TypeError(goal)

        return (
            propositions,
            actions,
            positive_goals,
            negative_goals,
        )

    def _proposition_to_str(self, proposition: Literal) -> str:
        predicate = proposition.predicate
        args = proposition.args
        if len(args) == 0:
            return f"({predicate})"
        ret = f"({predicate}"
        for arg in args:
            ret += f" {arg}"
        ret += ")"
        return ret

    def _compute_graph_representation(self) -> None:
        G = self._init_graph()

        (
            propositions,
            actions,
            positive_goals,
            negative_goals,
        ) = self._get_grounded_problem_info()

        """ nodes """
        for proposition in propositions:
            node_p = self._proposition_to_str(proposition)
            if proposition in positive_goals:
                x_p = SlgFeatures.POS_GOAL
                self._pos_goal_nodes.add(node_p)
            elif proposition in negative_goals:
                x_p = SlgFeatures.NEG_GOAL
                self._neg_goal_nodes.add(node_p)
            else:
                x_p = SlgFeatures.NON_GOAL
            G.add_node(node_p, x=x_p.value)

        for action in actions:
            node_a = action.name
            x_a = SlgFeatures.ACTION
            G.add_node(node_a, x=x_a.value)

        """ edges """
        for action in actions:
            a_node = action.name
            for proposition in action.precondition:
                p_node = self._proposition_to_str(proposition)
                assert p_node in G.nodes, f"{p_node} not in nodes"
                assert a_node in G.nodes, f"{a_node} not in nodes"
                G.add_edge(
                    u_of_edge=p_node,
                    v_of_edge=a_node,
                    edge_label=SlgEdgeLabels.PRE_EDGE.value,
                )
            # ignoring conditional effects
            for _, proposition in action.add_effects:
                p_node = self._proposition_to_str(proposition)
                assert p_node in G.nodes, f"{p_node} not in nodes"
                assert a_node in G.nodes, f"{a_node} not in nodes"
                G.add_edge(
                    u_of_edge=p_node,
                    v_of_edge=a_node,
                    edge_label=SlgEdgeLabels.ADD_EDGE.value,
                )
            # ignoring conditional effects
            for _, proposition in action.del_effects:
                p_node = self._proposition_to_str(proposition)
                assert p_node in G.nodes, f"{p_node} not in nodes"
                assert a_node in G.nodes, f"{a_node} not in nodes"
                G.add_edge(
                    u_of_edge=p_node,
                    v_of_edge=a_node,
                    edge_label=SlgEdgeLabels.DEL_EDGE.value,
                )

        # map node name to index
        self._node_to_i = {}
        for i, node in enumerate(G.nodes):
            self._node_to_i[node] = i
        self.G = G
        return

    def _colour_to_tensor(self, colour: int) -> Tensor:
        return self._one_hot_node(colour)
