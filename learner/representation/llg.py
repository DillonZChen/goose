""" LLG from AAAI-24 submission. """
import torch
from enum import Enum
from .planning.translate.pddl import Atom, NegatedAtom, Truth
from .base_class import (
    Representation,
    LiftedState,
    CGraph,
    TGraph,
    Tensor,
    AaaiAchievedWlColours,
)


class LlgFeatures(Enum):
    P = 0  # is predicate
    A = 1  # is action
    G = 2  # is positive goal (grounded)
    N = 3  # is negative goal (grounded)
    S = 4  # is activated (grounded)
    O = 5  # is object
    Z = 6  # no feature


class LlgEdgeLabels(Enum):
    NEUTRAL = 0
    GROUND = 1
    PRE_POS = 2
    PRE_NEG = 3
    EFF_POS = 4
    EFF_NEG = 5


INDEX_FEAT_SIZE = 4


class LiftedLearningGraph(Representation):
    name = "llg"
    lifted = True

    def __init__(self, domain_pddl: str, problem_pddl: str):
        super().__init__(
            domain_pddl,
            problem_pddl,
            n_node_features=len(LlgFeatures) + INDEX_FEAT_SIZE,
            n_edge_labels=len(LlgEdgeLabels),
        )

    def str_to_state(self, state) -> LiftedState:
        """Used in dataset construction to convert string representation of facts into a (pred, [args]) representation"""
        ret = []
        for fact in state:
            fact = fact.replace(")", "").replace("(", "")
            toks = fact.split()
            if toks[0] == "=":
                continue
            if len(toks) > 1:
                ret.append((toks[0], toks[1:]))
            else:
                ret.append((toks[0], ()))
        return ret

    def state_to_tgraph(self, state: LiftedState) -> TGraph:
        """States are represented as a list of (pred, [args])"""
        x = self.x.clone()
        edge_indices = self.edge_indices.copy()
        i = len(x)

        to_add = sum(len(fact[1]) + 1 for fact in state)
        x = torch.nn.functional.pad(x, (0, 0, 0, to_add), "constant", 0)
        append_edge_index = []

        for fact in state:
            pred = fact[0]
            args = fact[1]

            if len(pred) == 0:
                continue

            node = (pred, tuple(args))

            # activated proposition overlaps with a goal Atom or NegatedAtom
            if node in self._node_to_i:
                x[self._node_to_i[node]][LlgFeatures.S.value] = 1
                continue

            # activated proposition does not overlap with a goal
            true_node_i = i
            x[i][LlgFeatures.S.value] = 1
            i += 1

            # connect fact to predicate
            append_edge_index.append((true_node_i, self._node_to_i[pred]))
            append_edge_index.append((self._node_to_i[pred], true_node_i))

            # connect to predicates and objects
            for k, arg in enumerate(args):
                true_var_node_i = i
                x[i][-INDEX_FEAT_SIZE:] = self._if[k]
                i += 1

                # connect variable to predicate
                append_edge_index.append((true_node_i, true_var_node_i))
                append_edge_index.append((true_var_node_i, true_node_i))

                # connect variable to object
                append_edge_index.append(
                    (true_var_node_i, self._node_to_i[arg])
                )
                append_edge_index.append(
                    (self._node_to_i[arg], true_var_node_i)
                )

        edge_indices[LlgEdgeLabels.GROUND.value] = torch.hstack(
            (
                edge_indices[LlgEdgeLabels.GROUND.value],
                torch.tensor(append_edge_index).T,
            )
        ).long()

        return x, edge_indices

    def state_to_cgraph(self, state: LiftedState) -> CGraph:
        """States are represented as a list of (pred, [args])"""
        c_graph = self.G.copy()
        new_idx = len(self._node_to_i)

        for fact in state:
            pred = fact[0]
            args = fact[1]

            if len(pred) == 0:
                continue

            node = (pred, tuple(args))

            # activated proposition overlaps with a goal Atom or NegatedAtom
            if node in self._pos_goal_nodes:
                col = AaaiAchievedWlColours.ACH_POS_GOAL.value
                c_graph.nodes[node]["x"] = col
                continue
            elif node in self._neg_goal_nodes:
                col = AaaiAchievedWlColours.ACH_NEG_GOAL.value
                c_graph.nodes[node]["x"] = col
                continue

            node = new_idx

            # else add node and corresponding edges to graph
            col = AaaiAchievedWlColours.ACH_NON_GOAL.value
            c_graph.add_node(node, x=col)

            # connect fact to predicate
            assert pred in c_graph.nodes
            c_graph.add_edge(
                u_of_edge=node,
                v_of_edge=pred,
                edge_label=LlgEdgeLabels.GROUND.value,
            )
            c_graph.add_edge(
                v_of_edge=node,
                u_of_edge=pred,
                edge_label=LlgEdgeLabels.GROUND.value,
            )

            # connect to predicates and objects
            for k, obj in enumerate(args):
                new_idx += 1
                arg_node = new_idx

                c_graph.add_node(arg_node, x=-k)

                # connect variable to fact
                c_graph.add_edge(
                    u_of_edge=node,
                    v_of_edge=arg_node,
                    edge_label=LlgEdgeLabels.GROUND.value,
                )
                c_graph.add_edge(
                    v_of_edge=node,
                    u_of_edge=arg_node,
                    edge_label=LlgEdgeLabels.GROUND.value,
                )

                # connect variable to object
                assert obj in c_graph.nodes
                c_graph.add_edge(
                    u_of_edge=arg_node,
                    v_of_edge=obj,
                    edge_label=LlgEdgeLabels.GROUND.value,
                )
                c_graph.add_edge(
                    v_of_edge=arg_node,
                    u_of_edge=obj,
                    edge_label=LlgEdgeLabels.GROUND.value,
                )

            new_idx += 1

        return c_graph

    def _construct_if(self) -> None:
        """Precompute a seeded randomly generated injective index function"""
        self._if = []
        image = set()  # for checking injectiveness

        # TODO read max range from problem(s)
        for idx in range(10):
            torch.manual_seed(idx)
            rep = 2 * torch.rand(INDEX_FEAT_SIZE) - 1  # U[-1,1]
            rep /= torch.linalg.norm(rep)
            self._if.append(rep)
            key = tuple(rep.tolist())
            assert key not in image
            image.add(key)
        return

    def _if_feature(self, idx: int) -> Tensor:
        ret = torch.zeros(self.n_node_features)
        ret[-INDEX_FEAT_SIZE:] = self._if[idx]
        return ret

    def _compute_graph_representation(self) -> None:
        self._construct_if()

        G = self._init_graph()

        # objects
        for i, obj in enumerate(self.problem.objects):
            G.add_node(obj.name, x=LlgFeatures.O.value)

        # predicates
        largest_predicate = 0
        for pred in self.problem.predicates:
            largest_predicate = max(largest_predicate, len(pred.arguments))
            G.add_node(pred.name, x=LlgFeatures.P.value)

        # fully connected between objects and predicates
        for pred in self.problem.predicates:
            for obj in self.problem.objects:
                G.add_edge(
                    u_of_edge=pred.name,
                    v_of_edge=obj.name,
                    edge_label=LlgEdgeLabels.NEUTRAL.value,
                )

        # goal (state gets dealt with in state_to_tgraph)
        if len(self.problem.goal.parts) == 0:
            goals = [self.problem.goal]
        else:
            goals = self.problem.goal.parts
        for fact in goals:
            assert type(fact) in {Atom, NegatedAtom}

            # may have negative goals
            is_negated = type(fact) == NegatedAtom

            pred = fact.predicate
            args = fact.args
            goal_node = (pred, args)

            if is_negated:
                x = LlgFeatures.N.value
                self._neg_goal_nodes.add(goal_node)
            else:
                x = LlgFeatures.G.value
                self._pos_goal_nodes.add(goal_node)
            G.add_node(goal_node, x=x)  # add grounded predicate node

            for i, arg in enumerate(args):
                goal_var_node = (goal_node, i)
                G.add_node(goal_var_node, x=len(LlgFeatures) + i)

                # connect variable to predicate
                G.add_edge(
                    u_of_edge=goal_node,
                    v_of_edge=goal_var_node,
                    edge_label=LlgEdgeLabels.GROUND.value,
                )

                # connect variable to object
                assert arg in G.nodes()
                G.add_edge(
                    u_of_edge=goal_var_node,
                    v_of_edge=arg,
                    edge_label=LlgEdgeLabels.GROUND.value,
                )

            # connect grounded fact to predicate
            assert pred in G.nodes()
            G.add_edge(
                u_of_edge=goal_node,
                v_of_edge=pred,
                edge_label=LlgEdgeLabels.GROUND.value,
            )
        # end goal

        # actions
        largest_action_schema = 0
        for action in self.problem.actions:
            G.add_node(action.name, x=LlgFeatures.A.value)
            action_args = {}

            largest_action_schema = max(
                largest_action_schema, len(action.parameters)
            )
            for i, arg in enumerate(action.parameters):
                arg_node = (action.name, f"action-var-{i}")  # action var
                G.add_node(arg_node, x=len(LlgFeatures) + i)
                action_args[arg.name] = arg_node
                G.add_edge(
                    u_of_edge=action.name,
                    v_of_edge=arg_node,
                    edge_label=LlgEdgeLabels.NEUTRAL.value,
                )

            def deal_with_action_prec_or_eff(predicates, edge_label):
                for z, predicate in enumerate(predicates):
                    pred = predicate.predicate
                    # aux node for duplicate preds
                    aux_node = (pred, f"{edge_label}-aux-{z}")
                    G.add_node(aux_node, x=LlgFeatures.Z.value)

                    assert pred in G.nodes()
                    G.add_edge(
                        u_of_edge=pred,
                        v_of_edge=aux_node,
                        edge_label=edge_label,
                    )

                    if len(predicate.args) > 0:
                        for j, arg in enumerate(predicate.args):
                            prec_arg_node = (
                                arg,
                                f"{edge_label}-aux-{z}-var-{j}",
                            )
                            G.add_node(prec_arg_node, x=len(LlgFeatures) + j)
                            G.add_edge(
                                u_of_edge=aux_node,
                                v_of_edge=prec_arg_node,
                                edge_label=edge_label,
                            )

                            if arg in action_args:
                                action_arg_node = action_args[arg]
                                G.add_edge(
                                    u_of_edge=prec_arg_node,
                                    v_of_edge=action_arg_node,
                                    edge_label=edge_label,
                                )
                    else:  # unitary predicate so connect directly to action
                        G.add_edge(
                            u_of_edge=aux_node,
                            v_of_edge=action.name,
                            edge_label=edge_label,
                        )
                return

            pos_pres = []
            neg_pres = []
            pos_effs = []
            neg_effs = []

            for p in action.precondition.parts:
                if type(p) == Atom:
                    pos_pres.append(p)
                elif type(p) == NegatedAtom:
                    neg_pres.append(p)
            for p in action.effects:
                if type(p.condition) != Truth:
                    raise NotImplementedError(
                        "Conditional effects not implemented"
                    )
                if type(p.literal) == Atom:
                    pos_effs.append(p.literal)
                elif type(p.literal) == NegatedAtom:
                    neg_effs.append(p.literal)

            deal_with_action_prec_or_eff(pos_pres, LlgEdgeLabels.PRE_POS.value)
            deal_with_action_prec_or_eff(neg_pres, LlgEdgeLabels.PRE_NEG.value)
            deal_with_action_prec_or_eff(pos_effs, LlgEdgeLabels.EFF_POS.value)
            deal_with_action_prec_or_eff(neg_effs, LlgEdgeLabels.EFF_NEG.value)
        # end actions

        assert largest_predicate > 0
        assert largest_action_schema > 0

        # map node name to index
        self._node_to_i = {}
        for i, node in enumerate(G.nodes):
            self._node_to_i[node] = i
        self.G = G
        return

    def _colour_to_tensor(self, colour: int) -> Tensor:
        if colour < len(LlgFeatures):
            return self._one_hot_node(colour)
        else:
            return self._if_feature(colour - len(LlgFeatures))
