""" 
A wrapper for the WL pipeline that involves the feature generator and machine
learning model using generated features
"""

import traceback
import numpy as np
from typing import Iterable, List, Optional, Dict, Tuple, Union
from representation import CGraph, Representation, REPRESENTATIONS, State
from models.sml.core import LINEAR_MODELS, BaseModel
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
        self._rep_type: str = args.rep
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

    def get_representation(self) -> str:
        return self._rep_type
    
    def get_wl_algorithm(self) -> str:
        return self.wl_name

    def get_hash(self) -> Dict[str, int]:
        return self._wl.get_hash()
    
    def get_no_edge_colour(self) -> int:
        return NO_EDGE

    def compute_histograms(
        self, graphs: CGraph, return_ratio_seen_counts: bool = False
    ) -> Union[List[Histogram], Tuple[List[Histogram], List[float]]]:
        return self._wl.compute_histograms(graphs, return_ratio_seen_counts)

    def get_matrix_representation(
        self, graphs: CGraph, histograms: Optional[List[Histogram]]
    ) -> np.array:
        return self._wl.get_x(graphs, histograms)

    def combine_with_other_models(self, path_to_models: List[str]):
        from models.save_load import load_ml_model, save_ml_model

        # only works for linear models + same WL
        # TODO some code to do checks

        print(f"Combining with {len(path_to_models)} other linear models...")

        self._other_linear_models = []  # List[Tuple[np.array, double]]
        this_hash = self.get_hash()

        for path in path_to_models:
            model: Model = load_ml_model(path)

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

    def lifted_state_input(self) -> bool:
        return self._representation.lifted

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
        y = self.predict([x])[0]
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
        raise NotImplementedError
    
    def setup_after_loading(self, path, domain_file, problem_file) -> None:
        super().setup_after_loading(path, domain_file, problem_file)
        self.update_representation(domain_file, problem_file)
        return

    @property
    def n_colours_(self) -> int:
        return self._wl.n_colours_
