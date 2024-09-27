""" 
We make Graph a separate object from Representation with the idea that graphs
should be final variables, and let Representation do any manipulations with 
inputs, such as when interacting with tensors related to the GNN.

Thus, we can abstract away whatever mess is in here.

e.g. only one Graph object is ever created for each instance we try to solve
"""

import logging
from dataclasses import dataclass
from types import FunctionType
from typing import Dict, Iterable, List, Optional, Set, Tuple

import networkx as nx
import numpy as np
import torch
from torch import Tensor
from torch_geometric.utils.convert import from_networkx

from learner.deep_learning.nn.gnn_input import GnnInput
from learner.deep_learning.representation.feature_generator import (
    FactFeature, FeatureGenerator, FluentFeature, NumericalGoalFeature)
from learner.deep_learning.representation.label_generator import LabelGenerator
from learner.problem.numeric_condition import NumericCondition
from learner.problem.numeric_problem import NumericProblem
from learner.problem.numeric_state import NumericState
from learner.problem.util import var_to_objects, var_to_predicate


@dataclass
class ExpressionAndIndices:
    index: int
    neighbour_indices: np.array
    function: FunctionType


class Graph:
    def __init__(
        self,
        problem: NumericProblem,
        feature_generator: FeatureGenerator,
        label_generator: LabelGenerator,
        numeric_agnostic: bool,
    ):
        self.problem = problem

        # hyperparameters
        self.numeric_agnostic = numeric_agnostic
        self.feature_generator = feature_generator
        self.label_generator = label_generator
        self.n_node_features = feature_generator.n_features
        self.n_edge_labels = label_generator.n_labels

        # pyg data
        self.x: Tensor = None
        self.edge_indices: List[Tensor] = None

        # graph objects
        self._node_to_idx: Dict[str, int] = None
        self._idx_to_node: Dict[int, str] = None
        self._node_to_type: Dict[str, str] = None
        self._type_to_nodes: Dict[str, Set[str]] = None
        self._node_to_num_goal: Dict[str, NumericCondition] = None
        self.G: nx.Graph = None

        # initialise graph
        self._generate_graph()
        self._to_pyg()
        if self.G is not None and len(self.G.nodes) == 0:
            raise RuntimeError("Graph is empty or not generated properly.")
        self.n_nodes = len(self.G.nodes)
        self.n_edges = len(self.G.edges)
        self._test_generation()

        # other helpers
        self._str_bool_goals = set(self.problem.bool_goals)

    @property
    def node_to_idx(self) -> Dict[str, int]:
        return self._node_to_idx

    @property
    def bool_goals(self) -> Set[str]:
        return self._str_bool_goals

    def _generate_graph(self) -> None:
        G = nx.Graph()

        num_goals = sorted(self.problem.num_goals, key=repr)
        bool_goals = sorted(self.problem.bool_goals)

        self._node_to_idx = {}
        self._idx_to_node = {}
        self._node_to_type = {}
        self._type_to_nodes = {
            "object": set(),
            "num_var": set(),
            "num_goal": set(),
            "bool_goal": set(),
        }
        self._node_to_num_goal = {}

        def add_node(node: str, x: Tensor, node_type: str) -> None:
            if self.numeric_agnostic and node_type in {"num_var", "num_goal"}:
                return
            idx = len(G.nodes)
            assert isinstance(node, str)
            assert node not in self._node_to_idx
            assert idx not in self._idx_to_node
            self._node_to_idx[node] = idx
            self._idx_to_node[idx] = node
            self._node_to_type[node] = node_type
            self._type_to_nodes[node_type].add(node)
            G.add_node(node, x=x)

        def add_edge(node1: str, node2: str, label: int) -> None:
            if self.numeric_agnostic:
                nodes = self._type_to_nodes["object"] | self._type_to_nodes["bool_goal"]
                if node1 not in nodes or node2 not in nodes:
                    return
            else:
                nodes = G.nodes()
            assert node1 in nodes, node1
            assert node2 in nodes, node2
            G.add_edge(node1, node2, label=label)

        """ add nodes """
        # sort all objects to make it easier to debug tensors
        # objects
        for obj in sorted(self.problem.objects):
            assert isinstance(obj, str)
            node = obj
            x = self.feature_generator.feature(
                {
                    self.feature_generator.object_node_feat_idx(""): 1,
                }
            )
            add_node(node, x, node_type="object")

        # boolean goals (statics dealt with elsewhere)
        for bool_var in sorted(bool_goals):
            assert isinstance(bool_var, str)
            node = bool_var
            pred = var_to_predicate(node)
            desc = FactFeature.F_GOAL
            x = self.feature_generator.feature(
                {
                    self.feature_generator.fact_node_feat_idx(pred, desc): 1,
                }
            )
            add_node(node, x, node_type="bool_goal")

        # numerical variables
        for num_var in sorted(self.problem.fluents):
            assert isinstance(num_var, str)
            node = num_var
            pred = var_to_predicate(node)
            val = self.problem.initial_state.value(num_var)
            desc1 = FluentFeature.PREDICATE
            desc2 = FluentFeature.VALUE
            x = self.feature_generator.feature(
                {
                    self.feature_generator.fluent_node_feat_idx(pred, desc1): 1,
                    self.feature_generator.fluent_node_feat_idx(pred, desc2): val,
                }
            )
            add_node(node, x, node_type="num_var")

        # numerical goals
        for num_goal in sorted(num_goals, key=str):
            node = str(num_goal)
            desc1 = NumericalGoalFeature.IS_NUMERICAL_GOAL
            desc2 = NumericalGoalFeature.condition_type(num_goal.comparator)
            # NUMERICAL_GOAL_ACHIEVED and
            # NUMERICAL_GOAL_ERROR should be assigned at runtime
            x = self.feature_generator.feature(
                {
                    self.feature_generator.num_goal_node_feat_idx(desc1): 1,
                    self.feature_generator.num_goal_node_feat_idx(desc2): 1,
                }
            )
            self._node_to_num_goal[node] = num_goal
            add_node(node, x, node_type="num_goal")

        """ add edges """
        # boolean variables
        for bool_var in sorted(bool_goals):
            assert isinstance(bool_var, str)
            node = bool_var
            objects = var_to_objects(bool_var)
            for i, obj_node in enumerate(objects):
                label = self.label_generator.bool_var_arg_label(i)
                add_edge(node, obj_node, label)

        # numerical variables
        for num_var in sorted(self.problem.fluents):
            assert isinstance(num_var, str)
            node = num_var
            objects = var_to_objects(num_var)
            for i, obj_node in enumerate(objects):
                label = self.label_generator.num_var_arg_label(i)
                add_edge(node, obj_node, label)

        # numerical goals
        for num_goal in sorted(num_goals, key=repr):
            node = repr(num_goal)
            fluents = num_goal.get_variables()
            for f in fluents:
                label = self.label_generator.goal_label()
                add_edge(node, f, label)

        self.G = G

    def _to_pyg(self) -> GnnInput:
        pyg_G = from_networkx(self.G)

        x = pyg_G.x
        x = x.to(torch.float32)

        assert self.n_edge_labels >= 1
        edge_indices = [[] for _ in range(self.n_edge_labels)]
        edge_index_T = pyg_G.edge_index.T
        for i, edge_label in enumerate(pyg_G.label):
            edge_indices[edge_label].append(edge_index_T[i])
        for i in range(self.n_edge_labels):
            if len(edge_indices[i]) > 0:
                edge_indices[i] = torch.vstack(edge_indices[i]).long().T
            else:
                edge_indices[i] = torch.tensor([[], []]).long()

        self.x = x
        self.edge_indices = edge_indices

        return GnnInput(x=x, edge_indices=edge_indices)

    def _test_generation(self) -> None:
        assert len(self._node_to_idx) == self.n_nodes
        assert len(self._idx_to_node) == self.n_nodes
        assert len(self._node_to_type) == self.n_nodes

        # test nodes_by_type is a partition of G.nodes
        partition = set()
        for nodes in self._type_to_nodes.values():
            assert len(partition & nodes) == 0
            partition |= nodes
        assert partition == set(self.G.nodes)

        assert self.x is not None
        assert self.edge_indices is not None
        assert self.x.shape == (self.n_nodes, self.n_node_features)

    def update_from_statics(self, static_vars: List[str]) -> None:
        # only updates bool statics, all numerical variables are represented anyway
        x = self.x
        edge_indices = self.edge_indices

        static_vars = sorted(static_vars)
        facts = [var for var in static_vars if not self.problem.is_num_var(var)]
        self._update_from_facts(x, facts)

        self.x = self.feature_generator.add_cached_nodes(x)
        self.edge_indices = self.label_generator.add_cached_edges(edge_indices)

    def _update_from_facts(
        self, x: Tensor, facts: List[str], index_to_node: bool = False
    ) -> None:
        desc1 = FactFeature.T_GOAL
        desc2 = FactFeature.F_GOAL
        desc3 = FactFeature.T_FACT
        for var in facts:
            pred = var_to_predicate(var)
            if var in self._type_to_nodes["bool_goal"]:
                idx = self._node_to_idx[var]
                self.feature_generator.update_x(
                    x=x,
                    idx=idx,
                    assignments={
                        self.feature_generator.fact_node_feat_idx(pred, desc1): 1,
                        self.feature_generator.fact_node_feat_idx(pred, desc2): 0,
                    },
                )
            else:
                objects = var_to_objects(var)
                idx = self.feature_generator.cache_add_node(
                    x=x,
                    assignments={
                        self.feature_generator.fact_node_feat_idx(pred, desc3): 1,
                    },
                )
                if index_to_node:
                    self._tmp_idx_to_node[idx] = var
                for i, obj in enumerate(objects):
                    obj_idx = self._node_to_idx[obj]
                    label = self.label_generator.bool_var_arg_label(i)
                    self.label_generator.cache_add_edge(idx, obj_idx, label)

    def _update_from_fluents(
        self, x: Tensor, state: NumericState, index_to_node: bool = False
    ) -> None:
        # numerical variables
        desc = FluentFeature.VALUE
        for var, val in state.fluent_values.items():
            pred = var_to_predicate(var)
            idx = self._node_to_idx[var]
            self.feature_generator.update_x(
                x=x,
                idx=idx,
                assignments={
                    self.feature_generator.fluent_node_feat_idx(pred, desc): val,
                },
            )

        # numerical goals
        desc1 = NumericalGoalFeature.NUMERICAL_GOAL_ACHIEVED
        desc2 = NumericalGoalFeature.NUMERICAL_GOAL_ERROR
        for idx, achieved, error in self.nfd_state_to_num_goal_evaluations(state):
            self.feature_generator.update_x(
                x=x,
                idx=idx,
                assignments={
                    self.feature_generator.num_goal_node_feat_idx(desc1): achieved,
                    self.feature_generator.num_goal_node_feat_idx(desc2): error,
                },
            )

    def nfd_state_to_num_goal_evaluations(
        self, state: NumericState
    ) -> List[Tuple[int, int, float]]:
        ret = []

        for node in self._type_to_nodes["num_goal"]:
            idx = self._node_to_idx[node]
            goal = self._node_to_num_goal[node]
            expr = goal.nfd_evaluate_expr(state)
            error = goal.error(expr)
            achieved = goal.achieved(expr)
            ret.append((idx, achieved, error))

        return ret

    def state_to_gnn_input(
        self, state: NumericState, index_to_node: bool = False
    ) -> GnnInput:
        x = self.x.clone()
        edge_indices = self.edge_indices.copy()

        if index_to_node:
            self._tmp_idx_to_node = {k: v for (k, v) in self._idx_to_node.items()}

        self._update_from_facts(x, state.true_facts, index_to_node=index_to_node)

        if not self.numeric_agnostic:
            self._update_from_fluents(x, state, index_to_node=index_to_node)

        x = self.feature_generator.add_cached_nodes(x)
        edge_indices = self.label_generator.add_cached_edges(edge_indices)
        ret = GnnInput(x=x, edge_indices=edge_indices)

        return ret

    def get_expression_and_indices(self) -> List[ExpressionAndIndices]:
        # should be agnostic to whether Boolean facts are grounded or not
        num_conditions = []
        for node in self._type_to_nodes["num_goal"]:
            num_goal = self._node_to_num_goal[node]
            index = self._node_to_idx[node]
            neighbour_indices = []
            for neighbour_node in num_goal.get_variables():
                neighbour_index = self._node_to_idx[neighbour_node]
                neighbour_indices.append(neighbour_index)
            func = num_goal.get_error_function()
            num_conditions.append(
                ExpressionAndIndices(
                    index=index,
                    neighbour_indices=np.array(neighbour_indices).astype(int),
                    function=func,
                )
            )
        return num_conditions

    def visualise(self, gnn_input: GnnInput, output_file: str) -> None:
        from pyvis.network import Network

        N = Network(height="1350px", width="100%", notebook=True)
        N.toggle_hide_edges_on_drag(False)
        N.toggle_hide_nodes_on_drag(False)
        N.barnes_hut()

        ObjectNodeColour = "gray"
        NumVarNodeColour = "blue"
        NumGoalNodeColour = "orange"
        TGoalNodeColour = "purple"
        FGoalNodeColour = "gold"
        TFactNodeColour = "brown"

        NumVarArgEdgeColour = "red"
        BoolVarArgEdgeColour = "green"
        GoalEdgeColour = "black"

        i_to_node = {}

        for i, row in enumerate(gnn_input.x):
            label = self._tmp_idx_to_node[i]

            node = label
            colour = None
            i_to_node[i] = node

            if node in self._type_to_nodes["object"]:
                colour = ObjectNodeColour
            elif node in self._type_to_nodes["num_var"]:
                colour = NumVarNodeColour
                label = f"{label} -> {self.problem.initial_state.value(node)}"
            elif node in self._type_to_nodes["num_goal"]:
                colour = NumGoalNodeColour
            else:
                pred = var_to_predicate(node)
                for desc, col in zip(
                    [FactFeature.T_FACT, FactFeature.T_GOAL, FactFeature.F_GOAL],
                    [TFactNodeColour, TGoalNodeColour, FGoalNodeColour],
                ):
                    j = self.feature_generator.fact_node_feat_idx(pred, desc)
                    if row[j] == 1:
                        colour = col
                        break
                if colour is None:
                    # it is an error if this happens
                    logging.info(node)
                    for i, val in enumerate(row):
                        if val == 0:
                            continue
                        logging.info(f"{i}: {val}")
                    logging.info(row)
                    raise RuntimeError(f"Node {node} not found in any node set")
            N.add_node(node, label=label, color=colour)

        edge_colours = (
            [GoalEdgeColour]
            + [NumVarArgEdgeColour] * self.label_generator.max_fluent_arity
            + [BoolVarArgEdgeColour] * self.label_generator.max_fact_arity
        )
        assert len(edge_colours) == len(gnn_input.edge_indices)
        for col, edge_indices in zip(edge_colours, gnn_input.edge_indices):
            for i, j in edge_indices.T:
                N.add_edge(i_to_node[i.item()], i_to_node[j.item()], color=col)

        # change font size and colour nodes
        _size = 100
        for node in N.nodes:
            node["size"] = _size
            node["font"] = {"size": _size}

        if "." not in output_file:
            output_file += ".html"
        N.show(output_file)

    @property
    def num_objects(self):
        return len(self._type_to_nodes["object"])

    def dump(self):
        print(f"n_nodes: {self.n_nodes}")
        print(f"n_edges: {self.n_edges}")
        print(f"n_node_features: {self.n_node_features}")
        print(f"n_edge_labels: {self.n_edge_labels}")
