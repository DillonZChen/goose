from .base_class import *


class FLG_FEATURES(Enum):
    VAR = 0
    VAL = 1
    STATE = 2
    GOAL = 3
    ACTION = 4


class FLG_EDGE_LABELS(Enum):
    VV_EDGE = 0
    PRE_EDGE = 1
    EFF_EDGE = 2


class FdrLearningGraph(Representation, ABC):
    name = "flg"
    n_node_features = len(FLG_FEATURES)
    n_edge_labels = len(FLG_EDGE_LABELS)
    directed = False
    lifted = False

    def __init__(self, domain_pddl: str, problem_pddl: str) -> None:
        super().__init__(domain_pddl, problem_pddl)

    def _compute_graph_representation(self) -> None:
        """TODO: reference definition of this graph representation"""

        G = self._create_graph()

        variables = {}
        goal = self.problem.goal
        goals = 0

        for var, val in self.problem.varval_to_fact:  # see planning.representations.FDRProblem
            if var not in variables:
                variables[var] = set()
            variables[var].add(val)

        """ var val variables nodes and edges """
        for var in variables:
            G.add_node(var, x=self._one_hot_node(FLG_FEATURES.VAR.value))
            for val in variables[var]:
                val_node = (var, val)
                val_x = self._one_hot_node(FLG_FEATURES.VAL.value)

                if var in goal and val == goal[var]:
                    goals += 1
                    val_x += self._one_hot_node(FLG_FEATURES.GOAL.value)

                G.add_node(val_node, x=val_x)
                G.add_edge(
                    u_of_edge=var, v_of_edge=val_node, edge_label=FLG_EDGE_LABELS.VV_EDGE.value
                )
        assert goals == len(goal)

        """ action nodes and edges """
        for action in self.problem.actions:
            action_node = action.name
            G.add_node(action_node, x=self._one_hot_node(FLG_FEATURES.ACTION.value))
            for var, val in action.preconditions:
                assert val in variables[var]  # and hence should be in G.nodes()
                val_node = (var, val)
                assert val_node in G.nodes()
                G.add_edge(
                    u_of_edge=action_node,
                    v_of_edge=val_node,
                    edge_label=FLG_EDGE_LABELS.PRE_EDGE.value,
                )

            for var, val in action.add_effects:  # from our compilation, effects are in add only
                assert val in variables[var]
                val_node = (var, val)
                assert val_node in G.nodes()
                G.add_edge(
                    u_of_edge=action_node,
                    v_of_edge=val_node,
                    edge_label=FLG_EDGE_LABELS.EFF_EDGE.value,
                )

        # map node name to index
        node_to_i = {}
        for i, node in enumerate(G.nodes):
            node_to_i[node] = i
        self.fact_to_i = {}
        for fact in self.problem.fact_to_varval:
            self.fact_to_i[fact] = node_to_i[self.problem.fact_to_varval[fact]]
        self.G = G

        return

    def state_to_tensor(self, state: State) -> Tuple[Tensor, Tensor]:
        x = self.x.clone()
        for p in state:
            if p in self.problem.fact_to_varval:
                x[self.fact_to_i[p]][FLG_FEATURES.STATE.value] = 1

        return x, self.edge_indices

    def state_to_cgraph(self, state) -> CGraph:
        raise NotImplementedError
