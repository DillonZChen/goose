""" 
Implement my own vector representation of a graph instead of using a library given
that we spend most of the time with the vector representation anyway.
"""

from copy import deepcopy
from enum import Enum
from itertools import product
from pprint import pprint
from typing import Dict, List, Set, Tuple

from learner.problem.numeric_domain import NumericDomain
from learner.problem.numeric_problem import NumericProblem
from learner.problem.numeric_state import NumericState
from learner.problem.util import var_to_objects, var_to_predicate

Node = int
EdgeLabel = int
CatFeature = int  # categorical
ConFeature = float  # continuous

_T_GOAL = 0
_F_GOAL = 1
_T_FACT = 2


class FeatureGenerator:
    def __init__(self, domain_pddl: str):
        self.domain_pddl = domain_pddl
        self.domain = NumericDomain(domain_pddl)

        predicates: List[str] = list(map(str, self.domain.predicates))
        functions: List[str] = list(map(str, self.domain.functions))

        self._fact_pred_to_idx: Dict[str, int] = {
            p: i for i, p in enumerate(predicates)
        }
        self._fluent_pred_to_idx: Dict[str, int] = {
            p: i for i, p in enumerate(functions)
        }

        self._fact_f_offset = 1
        self._fluent_f_offset = self._fact_f_offset + 3 * len(self._fact_pred_to_idx)
        self._num_goal_f_offset = self._fluent_f_offset + len(self._fluent_pred_to_idx)

        self.n_features = self._num_goal_f_offset + 4

        ### detect constant objects
        constant_objects = []
        with open(domain_pddl, "r") as f:
            content = f.read()
            if "(:constants" in content:
                content = content[content.find("(:constants") + len("(:constants") :]
                content = content[: content.find(")")]
                lines = content.split("\n")
                for line in lines:
                    toks = line.split()
                    for tok in toks:
                        if tok == "-":
                            break
                        constant_objects.append(tok)
        constant_objects = sorted(constant_objects)
        self._constant_feature = {obj: -i - 1 for i, obj in enumerate(constant_objects)}

        self._test()

    def obj_feature(self, obj: str) -> CatFeature:
        if obj in self._constant_feature:
            return self._constant_feature[obj]
        return 0

    def fact_feature(self, pred: str, desc: int) -> CatFeature:
        return self._fact_f_offset + 3 * self._fact_pred_to_idx[pred] + desc

    def fluent_feature(self, pred: str) -> CatFeature:
        return self._fluent_f_offset + self._fluent_pred_to_idx[pred]

    def num_goal_feature(self, achieved: bool, comparator: str) -> CatFeature:
        if comparator in {"<=", "<"}:
            condition_type = 0
        else:
            assert comparator == "=="
            condition_type = 1
        desc = condition_type + 2 * int(achieved)  # binary
        return self._num_goal_f_offset + desc

    def feature_to_description(self, feature: CatFeature) -> str:
        if feature < self._fact_f_offset:
            return "object"
        elif feature < self._fluent_f_offset:
            feature -= self._fact_f_offset
            feature %= 3
            if feature == 0:
                return "T_GOAL"
            elif feature == 1:
                return "F_GOAL"
            else:
                return "T_FACT"
        elif feature < self._num_goal_f_offset:
            return "fluent"
        else:
            return "num_goal"

    def _test(self) -> None:
        feat_to_idx = {}
        feat_to_idx["object"] = self.obj_feature("_")
        for obj in self._constant_feature:
            idx = self.obj_feature(obj)
            assert idx not in feat_to_idx.items()
            feat_to_idx[obj] = idx
        name = {_F_GOAL: "F_GOAL", _T_GOAL: "T_GOAL", _T_FACT: "T_FACT"}
        for pred, desc in product(self._fact_pred_to_idx, [_F_GOAL, _T_GOAL, _T_FACT]):
            idx = self.fact_feature(pred, desc)
            assert 0 <= idx and idx < self.n_features
            assert idx not in feat_to_idx.items()
            feat_to_idx[f"{pred} {name[desc]}"] = idx
        for pred in self._fluent_pred_to_idx:
            idx = self.fluent_feature(pred)
            assert 0 <= idx and idx < self.n_features
            assert idx not in feat_to_idx.items()
            feat_to_idx[pred] = idx
        for achieved, comparator in product([False, True], {"<=", "=="}):
            idx = self.num_goal_feature(achieved, comparator)
            assert 0 <= idx and idx < self.n_features
            assert idx not in feat_to_idx.items()
            feat_to_idx[f"goal {achieved} {comparator}"] = idx
        return sorted(feat_to_idx.items(), key=lambda x: x[1])
    
    def features(self) -> None:
        return self._test()

    def dump(self) -> None:
        print("Init feature map:")
        for k, v in self._test():
            print(f"  {k} -> {v}")
        print(f"num init_features: {self.n_features}")


class Graph:
    def __init__(
        self,
        problem: NumericProblem,
        feature_generator: FeatureGenerator,
        generate_graph: bool = True,
    ) -> None:
        self.problem = problem

        self.n_features = feature_generator.n_features
        self.feature_generator: FeatureGenerator = feature_generator

        self.name_to_idx = {}
        self.idx_to_name = {}

        self.x_cat: List[CatFeature] = []
        self.x_con: List[ConFeature] = []
        self.neighbours: List[List[Tuple[Node, EdgeLabel]]] = []
        self.bool_goals: Set[str] = set(self.problem.bool_goals)

        if generate_graph:
            self._generate_graph()
            self._update_information()

    @property
    def nodes(self) -> List[Node]:
        return list(range(self.n_nodes))

    def _update_information(self) -> None:
        self.n_nodes = len(self.x_cat)
        self.n_edges = sum(len(neigh) for neigh in self.neighbours)
        self.degree = max(len(neigh) for neigh in self.neighbours)

    def add_node(self, name: str, cat: CatFeature, con: ConFeature) -> int:
        idx = len(self.name_to_idx)
        if name in self.name_to_idx:
            print(f"{name} should not be added again!!!")
            for k, v in self.name_to_idx.items():
                print(f"  {k} -> {v}")
            raise AssertionError()
        assert idx not in self.idx_to_name
        self.name_to_idx[name] = idx
        self.idx_to_name[idx] = name
        self.x_cat.append(cat)
        self.x_con.append(con)
        self.neighbours.append([])
        return idx

    def add_edge(self, name1: str, name2: str, label: EdgeLabel) -> None:
        assert name1 in self.name_to_idx
        assert name2 in self.name_to_idx
        idx1 = self.name_to_idx[name1]
        idx2 = self.name_to_idx[name2]
        self.neighbours[idx1].append((idx2, label))
        self.neighbours[idx2].append((idx1, label))

    def _generate_graph(self) -> None:
        num_goals = sorted(self.problem.num_goals, key=str)
        bool_goals = sorted(list(self.bool_goals))
        objects = sorted(self.problem.objects)
        fluents = sorted(self.problem.fluents)

        """ nodes """
        for obj in objects:
            assert isinstance(obj, str)
            self.add_node(
                obj,
                cat=(self.feature_generator.obj_feature(obj)),
                con=0,
            )

        for bool_var in bool_goals:
            assert isinstance(bool_var, str)
            pred = var_to_predicate(bool_var)
            self.add_node(
                bool_var,
                cat=self.feature_generator.fact_feature(pred, _F_GOAL),
                con=0,
            )

        for num_var in fluents:
            assert isinstance(num_var, str)
            node = num_var
            pred = var_to_predicate(node)
            val = self.problem.initial_state.value(num_var)
            self.add_node(
                node,
                cat=self.feature_generator.fluent_feature(pred),
                con=val,  # if static, this will be the only assignment
            )

        for goal in num_goals:
            name = repr(goal)
            self.add_node(
                name,
                # this will be reassigned at runtime anyway
                cat=0,
                con=0,
            )

        """ edges """
        for bool_var in bool_goals:
            node = bool_var
            objects = var_to_objects(bool_var)
            for i, obj in enumerate(objects):
                self.add_edge(name1=node, name2=obj, label=i)

        for num_var in fluents:
            assert isinstance(num_var, str)
            node = num_var
            objects = var_to_objects(num_var)
            for i, obj in enumerate(objects):
                self.add_edge(name1=node, name2=obj, label=i)

        for num_goal in num_goals:
            node = repr(num_goal)
            fluents: List[str] = num_goal.get_variables()
            for f in fluents:
                self.add_edge(name1=node, name2=f, label=0)

    def update_from_statics(self, static_vars: List[str]) -> None:
        # only updates bool statics, all numerical variables are represented anyway
        for var in static_vars:
            pred = var_to_predicate(var)
            # copied from below
            if var in self.bool_goals:
                cat = self.feature_generator.fact_feature(pred, _T_GOAL)
                self.x_cat[self.name_to_idx[var]] = cat
            else:
                objects = var_to_objects(var)
                cat = self.feature_generator.fact_feature(pred, _T_FACT)
                self.add_node(var, cat=cat, con=0)
                for i, obj in enumerate(objects):
                    self.add_edge(name1=var, name2=obj, label=i)

    def state_to_graph(self, state: NumericState) -> "Graph":
        graph = Graph(
            problem=self.problem,
            feature_generator=self.feature_generator,
            generate_graph=False,
        )

        # there's probably a better way to do this
        graph.name_to_idx = {k: v for k, v in self.name_to_idx.items()}
        graph.idx_to_name = {k: v for k, v in self.idx_to_name.items()}
        graph.x_cat = [v for v in self.x_cat]
        graph.x_con = [v for v in self.x_con]
        graph.neighbours = [[k for k in v] for v in self.neighbours]

        for var in state.true_facts:
            pred = var_to_predicate(var)
            if var in self.bool_goals:
                idx = graph.name_to_idx[var]
                cat = graph.feature_generator.fact_feature(pred, _T_GOAL)
                graph.x_cat[idx] = cat
            else:
                objects = var_to_objects(var)
                cat = graph.feature_generator.fact_feature(pred, _T_FACT)
                idx = graph.add_node(var, cat=cat, con=0)
                graph.name_to_idx[var] = idx
                graph.idx_to_name[idx] = var
                for i, obj in enumerate(objects):
                    graph.add_edge(name1=var, name2=obj, label=i)

        for var, val in state.fluent_values.items():
            idx = graph.name_to_idx[var]
            graph.x_con[idx] = val

        # assumes that num_goals, name_to_idx, feature_generator stays consistent
        for idx, cat, con in self.nfd_state_to_num_goal_evaluations(state):
            graph.x_cat[idx] = cat
            graph.x_con[idx] = con

        for goal in graph.problem.num_goals:
            name = repr(goal)
            expr = goal.nfd_evaluate_expr(state)
            error = goal.error(expr)
            achieved = goal.achieved(expr)
            comparator = goal.comparator
            idx = graph.name_to_idx[name]
            cat = graph.feature_generator.num_goal_feature(achieved, comparator)
            graph.x_cat[idx] = cat
            graph.x_con[idx] = error

        graph._update_information()
        return graph

    def nfd_state_to_num_goal_evaluations(
        self, state: NumericState
    ) -> List[Tuple[int, CatFeature, ConFeature]]:
        ret = []

        for goal in self.problem.num_goals:
            name = repr(goal)
            expr = goal.nfd_evaluate_expr(state)
            error = goal.error(expr)
            achieved = goal.achieved(expr)
            comparator = goal.comparator
            idx = self.name_to_idx[name]
            cat = self.feature_generator.num_goal_feature(achieved, comparator)
            ret.append((idx, cat, error))

        return ret

    def visualise(self, output_file) -> None:
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

        for node, i in self.name_to_idx.items():
            label = f"{node}\ncat={self.x_cat[i]}\ncon={self.x_con[i]}"
            desc = self.feature_generator.feature_to_description(self.x_cat[i])
            if desc == "object":
                colour = ObjectNodeColour
            elif desc == "fluent":
                colour = NumVarNodeColour
            elif desc == "num_goal":
                colour = NumGoalNodeColour
            elif desc == "T_GOAL":
                colour = TGoalNodeColour
            elif desc == "F_GOAL":
                colour = FGoalNodeColour
            elif desc == "T_FACT":
                colour = TFactNodeColour
            N.add_node(node, label=label, color=colour)

        for i, neighbours_i in enumerate(self.neighbours):
            for j, label in neighbours_i:
                N.add_edge(self.idx_to_name[i], self.idx_to_name[j], label=label)

        # change font size and colour nodes
        _size = 100
        for node in N.nodes:
            node["size"] = _size
            node["font"] = {"size": _size}

        if "." not in output_file:
            output_file += ".html"
        N.show(output_file)

    def dump(self, verbosity: int = 0) -> None:
        if verbosity > 0:
            print(f"n_nodes: {self.n_nodes}")
            print(f"n_edges: {self.n_edges}")
            print(f"degree: {self.degree}")
            if verbosity > 1:
                print("nodes:")
                for idx in self.nodes:
                    name = self.idx_to_name[idx]
                    cat = self.x_cat[idx]
                    con = self.x_con[idx]
                    print(f"{idx}\{name}: {cat}, {con}")
                print("edges:")
                for idx1 in self.nodes:
                    name1 = self.idx_to_name[idx1]
                    for idx2, label in self.neighbours[idx1]:
                        name2 = self.idx_to_name[idx2]
                        print(f"{idx1}\{name1} -> {idx2}\{name2}: {label}")
        else:
            print("x_cat", flush=True)
            for i, cat in enumerate(self.x_cat):
                print(f"{i}_cat: {int(cat)}", flush=True)
            print()
            print("x_con", flush=True)
            for i, con in enumerate(self.x_con):
                if con == round(con):
                    con = int(con)
                print(f"{i}_con: {con}", flush=True)
            print()
            print("neighbours", flush=True)
            for i, neigh in enumerate(self.neighbours):
                neigh = sorted(neigh)
                neigh_str = ""
                for u, v in neigh:
                    neigh_str += f"{u} "
                    neigh_str += f"{v} "
                print(f"{i}: {neigh_str}", flush=True)
            print()
