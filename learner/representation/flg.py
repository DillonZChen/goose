""" FLG from AAAI-24 submission. """
from enum import Enum
from .base_class import (
    Representation,
    GroundedState,
    CGraph,
    TGraph,
    Tensor,
    AaaiAchievedWlColours,
)


class FlgFeatures(Enum):
    ACTION = 0
    VAR = 1
    VAL = 2
    GOAL = 3
    ACH_PROP = 4


class FlgEdgeLabels(Enum):
    VAR_VAL_EDGE = 0
    PRE_EDGE = 1
    EFF_EDGE = 2


class FdrLearningGraph(Representation):
    name = "flg"
    lifted = False

    def __init__(self, domain_pddl: str, problem_pddl: str) -> None:
        super().__init__(
            domain_pddl,
            problem_pddl,
            n_node_features=len(FlgFeatures),
            n_edge_labels=len(FlgEdgeLabels),
        )

    def str_to_state(self, state) -> GroundedState:
        return state

    def state_to_tgraph(self, state: GroundedState) -> TGraph:
        x = self.x.clone()
        for p in state:
            x[self.fact_to_i[p]][FlgFeatures.ACH_PROP.value] = 1
        return x, self.edge_indices

    def state_to_cgraph(self, state: GroundedState) -> CGraph:
        c_graph = self.G.copy()
        for p in state:
            if p in self._pos_goal_nodes:
                colour = AaaiAchievedWlColours.ACH_POS_GOAL
            else:
                colour = AaaiAchievedWlColours.ACH_NON_GOAL
            p = self.problem.fact_to_varval[p]
            c_graph.nodes[p]["x"] = colour.value
        return c_graph

    def _compute_graph_representation(self) -> None:
        G = self._init_graph()

        variables = {}
        goal = self.problem.goal
        n_goals = 0

        # see planning.representations.FDRProblem
        for var, val in self.problem.varval_to_fact:
            if var not in variables:
                variables[var] = set()
            variables[var].add(val)

        """ var val variables nodes and edges """
        for var in variables:
            G.add_node(var, x=FlgFeatures.VAR.value)
            for val in variables[var]:
                val_node = (var, val)
                val_x = FlgFeatures.VAL.value

                if var in goal and val == goal[var]:
                    n_goals += 1
                    val_x = FlgFeatures.GOAL.value
                else:
                    val_x = FlgFeatures.VAL.value

                G.add_node(val_node, x=val_x)
                G.add_edge(
                    u_of_edge=var,
                    v_of_edge=val_node,
                    edge_label=FlgEdgeLabels.VAR_VAL_EDGE.value,
                )
        assert n_goals == len(goal)

        """ action nodes and edges """
        for action in self.problem.actions:
            action_node = action.name
            G.add_node(action_node, x=FlgFeatures.ACTION.value)
            for var, val in action.preconditions:
                assert val in variables[var]
                # and hence should be in G.nodes()

                val_node = (var, val)
                assert val_node in G.nodes()

                G.add_edge(
                    u_of_edge=action_node,
                    v_of_edge=val_node,
                    edge_label=FlgEdgeLabels.PRE_EDGE.value,
                )

            # from our compilation, effects are in add only
            for var, val in action.add_effects:
                assert val in variables[var]
                val_node = (var, val)
                assert val_node in G.nodes()
                G.add_edge(
                    u_of_edge=action_node,
                    v_of_edge=val_node,
                    edge_label=FlgEdgeLabels.EFF_EDGE.value,
                )

        # map node name to index
        node_to_i = {}
        for i, node in enumerate(G.nodes):
            node_to_i[node] = i
        self.G = G

        self.fact_to_i = {}
        for fact in self.problem.fact_to_varval:
            var_val = self.problem.fact_to_varval[fact]
            self.fact_to_i[fact] = node_to_i[var_val]
            var, val = var_val
            if var in goal and val == goal[var]:
                self._pos_goal_nodes.add(fact)
        return

    def _colour_to_tensor(self, colour: int) -> Tensor:
        return self._one_hot_node(colour)
