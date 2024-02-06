""" 
A wrapper for the WL pipeline that involves the feature generator and machine
learning model using generated features
"""

import random
import time
import traceback
import numpy as np
from tqdm import tqdm
from typing import Iterable, List, Optional, Dict, Tuple, Union
from sklearn.metrics import mean_squared_error
from sklearn.model_selection import train_test_split
from representation import CGraph, Representation, REPRESENTATIONS, State
from models.sml.core import LINEAR_MODELS, BaseModel
from util.stats import get_stats
from .base_wl import Histogram, NO_EDGE, WlAlgorithm
from .wl1 import ColourRefinement
from .wl2 import WL2
from .gwl2 import GWL2
from .lwl2 import LWL2
from .lwl3 import LWL3

WL_FEATURE_GENERATORS = {
    "1wl": ColourRefinement,
    "2wl": WL2,
    "2gwl": GWL2,
    "2lwl": LWL2,
    "3lwl": LWL3,
}

# prevent import so that we do not get pybind issues...
# used for deciding if we want to do schemata learning
# from dataset.factory import ALL_KEY
ALL_KEY = "_all_"


class Model(BaseModel):
    def __init__(self, args, schemata) -> None:
        super().__init__(args, schemata)
        self.wl_name = args.features

        self._iterations = args.iterations
        self._prune = args.prune
        self._rep_type = args.rep
        self._representation = None

        self._wl: WlAlgorithm = WL_FEATURE_GENERATORS[args.features](
            iterations=self._iterations,
            prune=self._prune,
        )

    def train(self) -> None:
        """set train mode, similar to pytorch models"""
        self._wl.train()

    def eval(self) -> None:
        """set eval mode, similar to pytorch models"""
        self._wl.eval()

    def get_hit_colours(self) -> int:
        return self._wl.get_hit_colours()

    def get_missed_colours(self) -> int:
        return self._wl.get_missed_colours()

    def _transform_for_mip(self, X) -> np.array:
        assert self.model_name == "mip"
        wl_hash = self.get_hash()
        n, d = X.shape
        additional = np.zeros((1, d))
        assert d == len(wl_hash)
        for k, v in wl_hash.items():
            assert 0 <= v < d
            toks = [int(t) for t in k.split(",")]
            node_colours = [toks[0]] + [
                toks[i] for i in range(1, len(toks), 2)
            ]

            edge_colours = [toks[i] for i in range(2, len(toks), 2)]

            # tiebreaker = len(toks)  # can be less naive (e.g. level of iteration)
            # diversity = len(set([(c,'n') for c in node_colours]+[(c,'e') for c in edge_colours]))

            # tiebreaker = 1 + int(-1 in edge_colours)  # want to maximise this
            # tiebreaker = 1 / tiebreaker  # because we minimise in mip
            if -1 in edge_colours:
                tiebreaker = 1
            else:
                tiebreaker = 10

            additional[0][v] = tiebreaker
        X = np.vstack((X, additional))
        return X

    def update_representation(
        self, domain_pddl: str, problem_pddl: str
    ) -> None:
        if (
            self._representation is not None
            and domain_pddl == self._representation.domain_pddl
            and problem_pddl == self._representation.problem_pddl
        ):
            return
        self._representation: Representation = REPRESENTATIONS[self._rep_type](
            domain_pddl, problem_pddl
        )
        return

    def get_iterations(self) -> int:
        return self._wl.iterations

    def get_hash(self) -> Dict[str, int]:
        return self._wl.get_hash()

    def compute_histograms(
        self, graphs: CGraph, return_ratio_seen_counts: bool = False
    ) -> Union[List[Histogram], Tuple[List[Histogram], List[float]]]:
        return self._wl.compute_histograms(graphs, return_ratio_seen_counts)

    def get_matrix_representation(
        self, graphs: CGraph, histograms: Optional[List[Histogram]]
    ) -> np.array:
        return self._wl.get_x(graphs, histograms)

    def combine_with_other_models(self, path_to_models: List[str]):
        from models.save_load import load_kernel_model, save_kernel_model

        # only works for linear models + same WL
        # TODO some code to do checks

        print(f"Combining with {len(path_to_models)} other linear models...")

        self._other_linear_models = []  # List[Tuple[np.array, double]]
        this_hash = self.get_hash()

        for path in path_to_models:
            model: Model = load_kernel_model(path)

            # other model is actually linear
            assert model.model_name in LINEAR_MODELS

            # so indicies of features are the same
            other_hash = model.get_hash()
            assert this_hash == other_hash

            weights = model.get_weights()
            bias = model.get_bias()
            self._other_linear_models.append((weights, bias))

        print("Combination successful!")
        return

    """ Methods called from cpp """

    def write_model_data(self) -> None:
        print("Writing model data to file...", flush=True)
        try:
            from datetime import datetime

            write_weights = self.model_name in LINEAR_MODELS

            df = self._representation.domain_pddl
            pf = self._representation.problem_pddl
            t = datetime.now().strftime("%Y-%m-%d-%H-%M-%S")
            file_path = "_".join(["graph", df, pf, t])
            file_path = repr(hash(file_path)).replace("-", "0")
            file_path = file_path + ".model"

            model_hash = self.get_hash()
            iterations = self.get_iterations()

            if write_weights:
                weights = self.get_weights()
                bias = self.get_bias()

                zero_weights = np.count_nonzero(weights == 0)
                print(
                    f"{zero_weights}/{len(weights)} = {zero_weights/len(weights):.2f} are zero"
                )

                assert len(weights) == len(model_hash)

                for k, v in model_hash.items():
                    assert (
                        0 <= v < len(weights)
                    ), f"{v} not in [0, {len(weights) - 1}]"

            n_linear_models = 0
            if (
                hasattr(self, "_other_linear_models")
                and self._other_linear_models is not None
            ):
                n_linear_models = len(self._other_linear_models)

            # write data
            with open(file_path, "w") as f:
                f.write(f"{NO_EDGE} NO_EDGE\n")
                f.write(f"{self._rep_type} representation\n")
                f.write(f"{self.wl_name} wl_algorithm\n")
                f.write(f"{iterations} iterations\n")

                f.write(f"{len(model_hash)} hash size\n")
                for k in model_hash:
                    f.write(f"{k} {model_hash[k]}\n")

                f.write(f"{n_linear_models + 1} linear model(s)\n")

                if write_weights:
                    n_weights = len(weights)

                    list_of_weights = [weights]  # n_models x n_weights
                    list_of_bias = [bias]  # n_models

                    if n_linear_models > 0:
                        for model in self._other_linear_models:
                            list_of_weights.append(model[0])
                            list_of_bias.append(model[1])

                    list_of_weights = np.vstack(
                        list_of_weights
                    ).T  # n_weights x n_models
                    assert list_of_weights.shape == (
                        n_weights,
                        n_linear_models + 1,
                    )

                    f.write(f"{n_weights} weights size\n")
                    for weights in list_of_weights:
                        f.write(
                            " ".join([str(w) for w in weights.tolist()]) + "\n"
                        )
                    f.write(
                        f"{' '.join([str(b) for b in list_of_bias])} bias\n"
                    )

            self._model_data_path = file_path
        except Exception:
            print(traceback.format_exc(), flush=True)

    def lifted_state_input(self) -> bool:
        return self._representation.lifted

    def get_model_data_path(self) -> str:
        print("Getting model file path...", flush=True)
        try:
            return self._model_data_path
        except Exception:
            print(traceback.format_exc(), flush=True)

    def write_representation_to_file(self) -> None:
        print("Writing representation to file...", flush=True)
        try:
            self._representation.write_to_file()
        except Exception:
            print(traceback.format_exc(), flush=True)
        return

    def get_graph_file_path(self) -> str:
        print("Getting representation file path...", flush=True)
        try:
            return self._representation.get_graph_file_path()
        except Exception:
            print(traceback.format_exc(), flush=True)

    def h(self, state: State) -> float:
        h = self.h_batch([state])[0]
        return h

    def h_batch(self, states: List[State]) -> List[float]:
        graphs = [
            self._representation.state_to_cgraph(state) for state in states
        ]
        X = self._wl.get_x(graphs)
        y = self.predict(X)
        hs = np.rint(y).astype(int).tolist()
        return hs

    def predict_h(self, x: Iterable[float]) -> float:
        """predict for single row x"""
        y = self.predict([x])
        # try:
        #     y = self.predict([x])
        # except Exception:
        #     print(traceback.format_exc(), flush=True)
        return y

    def predict_h_with_std(self, x: Iterable[float]) -> Tuple[float, float]:
        y, std = self.predict_with_std([x])
        return (y, std)

    def compute_std(self, x: Iterable[float]) -> float:
        _, std = self.predict_with_std([x])
        return std

    def online_training(
        self,
        states: List[State],
        ys: List[int],
        domain_pddl: str,
        problem_pddl: str,
    ) -> str:
        # returns new model data path (contains hash and weights)

        try:
            from dataset.wlf import get_dataset_from_args

            assert len(states) == len(ys)
            self.train()

            print("Loading initial training data...")
            graphs, y_true = get_dataset_from_args(self._args)
            graphs_train, _, y_train, _ = train_test_split(
                graphs, y_true, test_size=0.33, random_state=2023
            )
            y_train = [y[ALL_KEY] for y in y_train]
            print(f"Initial training data has {len(graphs_train)} graphs")

            print("Updating model representation...")
            self.update_representation(domain_pddl, problem_pddl)

            _SELECTION = 0.5
            print(
                f"Generating training data from {int(len(graphs_train) * _SELECTION)} out of {len(states)} new states..."
            )
            new_data = list(zip(states, ys))
            random.seed(2023)
            random.shuffle(new_data)
            new_data = new_data[: int(len(graphs_train) * _SELECTION)]
            new_ys = []
            for s, y in tqdm(new_data):
                ## cpp code already converts states to (pred, [args]) form
                graph = self._representation.state_to_cgraph(s)
                graphs_train.append(graph)
                new_ys.append(y)
            y_train = np.concatenate((y_train, np.array(new_ys)))

            # log dataset stats
            get_stats(
                dataset=list(zip(graphs_train, y_train)),
                desc="Online train dataset",
            )

            # try updating iterations
            # self._wl.update_iterations(self.iterations * 4)

            print("Generating histograms...")
            train_histograms = self.compute_histograms(
                graphs_train, return_ratio_seen_counts=False
            )

            print("Generating matrix...")
            X_train = self.get_matrix_representation(
                graphs_train, train_histograms
            )

            t = time.time()
            print("Training...")
            self.fit(X_train, y_train)
            print(f"Training complete in {time.time() - t:.2f}s!")

            y_train_pred = self.predict(X_train)
            mse = mean_squared_error(y_train_pred, y_train)
            print("mse:", mse)

            print("Writing model data...")
            self.write_model_data()

            self.eval()
            print(f"New hash size:", len(self.get_hash()))
            print("Python online training completed!", flush=True)
            return self._model_data_path
        except Exception:
            print(traceback.format_exc(), flush=True)

    def setup_for_saving(self, save_file: str) -> None:
        pass

    def setup_after_loading(self, save_file: str) -> None:
        pass

    @property
    def n_colours_(self) -> int:
        return self._wl.n_colours_
