"""
Big wrapper for the ML model and the representation. It is also the object that
is called from NFD (c++) through pybind.
"""

import logging
import traceback
from argparse import Namespace
from typing import Dict, List, Optional, Set, Tuple

import numpy as np
from sklearn.base import BaseEstimator
from tqdm import trange

from learner.dataset.dataset import Dataset
from learner.dataset.ranking_dataset import RankingDataset
from learner.evaluation_info import EvaluationInfo
from learner.feature_generation.estimator.mip_rk import MipRk
from learner.feature_generation.representation.graph import CatFeature, ConFeature
from learner.feature_generation.representation.numeric_wl import Hash, NumericWl
from learner.feature_generation.representation.wlf_representation import \
    CCwlRepresentation
from learner.model import Model
from learner.problem.numeric_state import NumericState
from learner.problem.util import var_to_objects, var_to_predicate
from util.set_cover_bnb import BB
from util.timer import TimerContextManager

logging.basicConfig(
    level=logging.INFO,
    # level=logging.DEBUG,
    format="%(levelname)s [%(filename)s:%(lineno)s] %(message)s",
)


class FeatureGenerationModel(Model):
    def __init__(self) -> None:
        super().__init__()
        self.representation: CCwlRepresentation = None
        self._estimator: Optional[BaseEstimator] = None
        self.train_info: EvaluationInfo = None
        self._weights: Dict[str, np.array] = {}
        self._min_pairs: List[Tuple[int, int]] = []

        # these variables only get filled for combined models
        self._round_opt_list: List[bool] = []  # n_models
        self._target_list: List[str] = []  # n_models
        self._weights_list: List[np.array] = []  # n_models
        self._min_pairs_list: List[List[Tuple[int, int]]] = []  # n_models

        self._n_init_features: int = None
        self._n_con_features: int = None
        self._n_cat_features: int = None

        self._hash = None

    # Python cannot overload methods e.g. for initialising a model with
    # arguments or initialising by loading from file both through __init__
    def set_objects(
        self,
        opts: Namespace,
        representation: CCwlRepresentation,
        estimator: BaseEstimator,
    ) -> None:
        self.opts = opts
        self.representation = representation
        self._estimator = estimator

    @property
    def n_features(self) -> int:
        return self.get_n_cat_features() + self.get_n_con_features()

    def update_estimator_from_dataset(self, dataset: Dataset) -> None:
        if self.opts.estimator_name in {"miprk"}:
            assert isinstance(dataset, RankingDataset)
            assert isinstance(self._estimator, MipRk)
            self._estimator.set_succ_groups(dataset.succ_groups)

    def fit(self, X: np.array, y: Dict[str, np.array]) -> Dict[str, float]:
        estimator_name = self.opts.estimator_name
        training_times = {}
        for schemata_key, y_schemata in y.items():
            if set(y_schemata) == {1} and estimator_name not in {"miprk"}:
                logging.info(f"Skipping {schemata_key} due to constant target of 1")
                self._weights[schemata_key] = np.ones(X.shape[1])
                continue
            if set(y_schemata) == {0} and estimator_name not in {"miprk"}:
                logging.info(f"Skipping {schemata_key} due to constant target of 0")
                self._weights[schemata_key] = -np.ones(X.shape[1])
                continue

            with TimerContextManager(f"fitting model for {schemata_key} cost") as timer:
                self._estimator.fit(X, y_schemata)
                training_times[schemata_key] = timer.get_time()
                if estimator_name in {"gpr"}:
                    weights = self._estimator.alpha_ @ self._estimator.X_train_
                elif estimator_name in {"gpc"}:
                    ## see sklearn.gaussian_process._gpc._BinaryGaussianProcessClassifierLaplace.predict
                    base_estimator = self._estimator.base_estimator_
                    assert base_estimator.classes_[0] < base_estimator.classes_[1]

                    ## assuming linear kernel
                    X_train = base_estimator.X_train_  # n x d
                    y_train = base_estimator.y_train_  # n x 1
                    pi = base_estimator.pi_  # n x 1  (predictions)
                    weights = X_train.T @ (y_train - pi)  # d x 1
                else:
                    weights = self._estimator.coef_
                    if estimator_name in {"mipcp"}:
                        self._min_pairs = self._estimator.pair_mins

                weights_shape = weights.shape
                intended_shape = (X.shape[1] + len(self._min_pairs),)
                assert (
                    weights_shape == intended_shape
                ), f"{weights_shape} != {intended_shape}"
                self._weights[schemata_key] = weights

        logging.info(f"Estimators fitted successfully!")
        return training_times

    @staticmethod
    def _stack_min(X: np.array, min_pairs: List[Tuple[int, int]]) -> np.array:
        if len(min_pairs) > 0:
            min_pair_columns = []
            for i, j in min_pairs:
                X_ci = X[:, i]
                X_cj = X[:, j]
                min_pair_columns.append(np.minimum(X_ci, X_cj))
            return np.hstack((X, np.vstack(min_pair_columns).T))
        else:
            return X
        
    def get_hash(self) -> Dict[str, int]:
        return self.representation.numeric_wl.get_hash()

    def predict(self, X: np.array, schemata_key: str) -> np.array:
        X = self._stack_min(X, self._min_pairs)
        pred = X @ self._weights[schemata_key]
        if self.opts.target in {"h", "r"}:
            return pred
        elif self.opts.target in {"d", "p"}:
            return pred > 0
        else:
            raise NotImplementedError(f"Not implemented for target {self.opts.target}")

    def save(self, save_file: str, model_name: Optional[str] = None) -> None:
        # save some memory
        self._estimator = None
        super().save(save_file, model_name)

    def load(self, load_file: str) -> None:
        super().load(load_file)
        self.representation.numeric_wl.eval()
        logging.info(f"Model loaded successfully from {load_file}")

    def dump(self) -> None:
        self.representation.dump()

    """ Methods here are mainly helpers for combining models """

    def get_weights(self) -> List[List[float]]:
        if self._weights is None:
            print(
                "Weights is None. You should not call this if this is a multi-heuristic model",
                flush=True,
            )
            exit(-1)
        if self.opts.estimator_name == "mipeq" and self.opts.schemata_strategy != "all":
            ## Schemata count model. This isn't really used anymore
            weights = np.zeros_like(next(iter(self._weights.values())))

            ## Set cover on schemata subsets with f1 == 1.0
            schemata = set()
            sel_sch = []
            for k in self._weights:
                if self.train_info.val_scores[k]["f1"] == 1.0:
                    schemata.update(k.split(","))
                    sel_sch.append(k)
            sch_to_idx = {s: i + 1 for i, s in enumerate(sorted(list(schemata)))}
            idx_sets = [[sch_to_idx[s] for s in ss.split(",")] for ss in sel_sch]

            print(f"Possible schemata: {schemata}")
            if len(idx_sets) > 1:
                with TimerContextManager("computing set cover of schemata subsets"):
                    universe = set(i for i in range(1, len(schemata) + 1))
                    costs = [1] * len(idx_sets)
                    opt_cost, opt_subsets = BB(universe, idx_sets, costs)

                for schemata_k, chosen in zip(sel_sch, opt_subsets):
                    if chosen:
                        print("Selected", schemata_k)
                        weights += self._weights[schemata_k]
            elif len(idx_sets) == 1:
                schemata_k = sel_sch[0]
                print("Selected", schemata_k)
                weights = self._weights[schemata_k]
        elif self.opts.target == "p":
            weights = []
            schemata = sorted(self._weights.keys())
            for k in schemata:
                weights.append(self._weights[k])
        else:
            weights = [sum(self._weights.values())]
        return weights

    @property
    def multi_heuristics(self) -> bool:
        target_list = self.get_targets_list()
        return sum([target == "h" for target in target_list]) > 1

    @property
    def pref_schema(self) -> bool:
        target_list = self.get_targets_list()
        return sum([target == "p" for target in target_list]) > 0

    def combine_models(self, paths: List[str]) -> None:
        self._weights = None
        self._hash = None
        for path in paths:
            model = FeatureGenerationModel()
            model.load(path)
            hash_ = model.get_hash()
            if self._hash is None:
                self._hash = hash_
            assert hash_ == self._hash
            weights = model.get_weights()
            if np.linalg.norm(weights) == 0:
                print(f"Skipping {path} due to 0 weights")
                continue

            # All models have a target and set of weights
            self._weights_list += weights
            for _ in range(len(weights)):
                self._round_opt_list.append(model.opts.round)
                self._target_list.append(model.opts.target)
                self._min_pairs_list.append(model._min_pairs)
            # print(f"{weights=}", flush=True)

        # This is only used to build the graph and evaluate numerical goals.
        # Thus, it doesn't matter which model we take it from
        self.representation = model.representation

        # These should be the same regardless of training data
        self._n_init_features = self.representation.feature_generator.n_features
        self._n_con_features = self.representation.numeric_wl.n_con_features

    """ Methods below are called from NFD (c++) through pybind """

    def set_domain_problem(
        self, domain_pddl: str, problem_pddl: str, nfd_vars_string: str
    ) -> None:
        super().set_domain_problem(domain_pddl, problem_pddl, nfd_vars_string)
        try:
            self.representation.set_problem(self.problem)
        except:
            traceback.print_exc()
            print("", flush=True)
            exit(-1)
        logging.info(f"Domain and problem set successfully!")

    def get_cat_iterations(self) -> int:
        return self.representation.numeric_wl.cat_iterations

    def get_con_iterations(self) -> int:
        return self.representation.numeric_wl.con_iterations

    def get_round_opts_list(self) -> List[bool]:
        if len(self._round_opt_list) == 0:
            self._round_opt_list = [self.opts.round]
        return self._round_opt_list

    def get_weights_list(self) -> List[List[float]]:
        if len(self._weights_list) == 0:
            self._weights_list = self.get_weights()
        return self._weights_list

    def get_weights_single(self) -> List[float]:
        weights = self.get_weights()
        if len(weights) != 1:
            print(f"Expected 1 set of weights, got {len(weights)}", flush=True)
            exit(-1)
        return weights[0]

    def get_min_pairs_list(self) -> List[List[Tuple[int, int]]]:
        if len(self._min_pairs_list) == 0:
            self._min_pairs_list = [self._min_pairs]
        return self._min_pairs_list

    def get_hash(self) -> Dict[str, int]:
        if self._hash is None:
            self._hash = self.representation.numeric_wl.get_hash()
        return self._hash

    def get_targets_list(self) -> List[str]:
        if len(self._target_list) == 0:
            self._target_list = [self.opts.target]
        return self._target_list

    def get_n_init_features(self) -> int:
        if self._n_init_features is None:
            self._n_init_features = self.representation.feature_generator.n_features
        return self._n_init_features

    def get_n_con_features(self) -> int:
        if self._n_con_features is None:
            self._n_con_features = self.representation.numeric_wl.n_con_features
        return self._n_con_features

    def get_n_cat_features(self) -> int:
        if self._n_cat_features is None:
            self._n_cat_features = self.representation.numeric_wl.n_cat_features
        return self._n_cat_features

    def get_schema_to_index(self) -> Dict[str, int]:
        ret = self.problem.schema_to_index
        return ret

    ### To give C++ graph information, and to evaluate
    def get_name_to_idx(self) -> Dict[str, int]:
        ret = {}
        for k, v in self.representation.graph.name_to_idx.items():
            # print(k, v, flush=True)
            ret[k] = v
            if "(" not in k:  # unified_planning being annoying
                k = f"{k}()"
                # print(k, v, flush=True)
                ret[k] = v
        return ret

    def get_x_cat(self) -> List[int]:
        return self.representation.graph.x_cat

    def get_x_con(self) -> List[float]:
        return self.representation.graph.x_con

    def get_neighbours(self, node: int) -> List[Tuple[int, int]]:
        return self.representation.graph.neighbours[node]

    def get_bool_goals(self) -> Set[str]:
        return self.representation.graph.bool_goals

    def get_fluents(self) -> List[str]:
        return super().get_fluents()

    def get_num_goal_updates(
        self, true_bools: List[str], num_vals: List[float]
    ) -> List[Tuple[int, CatFeature, ConFeature]]:
        nfd_state = NumericState(true_bools, dict(zip(self._nfd_fluents, num_vals)))
        ret = self.representation.graph.nfd_state_to_num_goal_evaluations(nfd_state)
        return ret

    ### use Python to get graph, and C++ to run WL [SLOW]
    def to_graph(self, true_bools: List[str], num_vals: List[float]) -> None:
        nfd_state = NumericState(true_bools, dict(zip(self._nfd_fluents, num_vals)))
        graph = self.representation.state_to_graph(nfd_state)
        self._tmp_state_graph = graph

    def get_state_x_cat(self) -> List[int]:
        return self._tmp_state_graph.x_cat

    def get_state_x_con(self) -> List[float]:
        return self._tmp_state_graph.x_con

    def get_state_neighbours(self, node: int) -> List[Tuple[int, int]]:
        return self._tmp_state_graph.neighbours[node]

    def get_pred_to_idx(self) -> Dict[str, int]:
        try:
            ret = self.representation.graph.feature_generator._fact_pred_to_idx
            return ret
        except:
            traceback.print_exc()
            print("", flush=True)
            exit(-1)

    def X_from_nfd(self, true_bools: List[str], num_vals: List[float]) -> np.array:
        nfd_state = NumericState(true_bools, dict(zip(self._nfd_fluents, num_vals)))
        graph = self.representation.state_to_graph(nfd_state)
        # graph.dump()
        X = self.representation.compute_features([graph])
        return np.array(X)

    ### do everything in Python [SLOWEST]
    def predict_deadend(self, true_bools: List[str], num_vals: List[float]) -> bool:
        y = False
        try:
            X = self.X_from_nfd(true_bools, num_vals)
            for i, target in enumerate(self.get_targets_list()):
                if target == "d":
                    X_i = self._stack_min(X, self.get_min_pairs_list()[i])
                    pred = X_i @ self._weights_list[i]
                    # print(f"{self._weights_list[i]=}", flush=True)
                    # print(pred, flush=True)
                    pred = pred > 0
                    y = y or pred
        except:
            traceback.print_exc()
            print("", flush=True)
            exit(-1)
        return y

    def evaluate(self, true_bools: List[str], num_vals: List[float]) -> float:
        try:
            # assume min list is empty
            X = self.X_from_nfd(true_bools, num_vals)
            y = X @ self.get_weights_list()[0]
            if self.get_round_opts_list()[0]:
                y = np.rint(y)
        except:
            traceback.print_exc()
            print("", flush=True)
            exit(-1)
        return y

    def evaluate_batch(
        self, list_true_bools: List[List[str]], list_num_vals: List[List[float]]
    ) -> List[float]:
        ys = []
        for true_bools, num_vals in zip(list_true_bools, list_num_vals):
            y = self.evaluate(true_bools, num_vals)
            ys.append(y)
        return y
