import os
from typing import Optional, Union

from _wlplan.feature_generation import _WLFeatures
from _wlplan.planning import Atom, Domain


class WLFeatures(_WLFeatures):
    """WL Feature Generator object.

    The implementation supports graphs with node colours and edge labels.

    Parameters
    ----------
        domain : Domain

        graph_representation : "ilg" or None, default="ilg"
            The graph encoding of planning states used. If None, the user can only call class method of classes and not datasets and states.

        iterations : int, default=2
            The number of WL iterations to perform.

        prune_features : "collapse", "collapse_by_layer" or None, default=None
            How to detect and prune duplicate features. If None, no pruning is done.

        multiset_hash : bool, default=False
            Choose to use either set or multiset to store neighbour colours.

    Methods
    -------
        collect(self, dataset: Dataset) -> None
            Collect training colours from dataset.

        collect(self, graphs: List[Graph]) -> None
            Collect training colours from graphs.

        set_problem(self, problem: Problem) -> None
            Set problem for graph generator if it exists. This should be called before calling `embed` on a state.

        embed(self, dataset: Dataset) -> list[int]
            Converts a dataset into a 2D matrix in the form of a list of lists. Throws an error if training colours have not been collected by calling `collect`.

        embed(self, graphs: List[Graph]) -> list[int]
            Converts a list of graphs into a 2D matrix in the form of a list of lists. Throws an error if training colours have not been collected by calling `collect`.

        embed(self, state: State) -> list[int]
            Converts a state into a list. Throws an error if training colours have not been collected by calling `collect`. An error may also occur if the state does not belong to the problem set by `set_problem`, or if `set_problem` is not called beforehand.

        get_n_features(self) -> int
            Returns number of collected features after pruning.

        get_seen_counts(self) -> List[int]
            Returns a list of length `iterations` with the count of seen features at each iteration. Counts are from seen colours collected from `collect` calls. The values are collected over all `embed` calls from the initialisation of this class.

        get_unseen_counts(self) -> List[int]
            Returns a list of length `iterations` with the count of unseen colours at each iteration. Counts are from colours not seen from `collect` calls. The values are collected over all `embed` calls from the initialisation of this class.

        get_n_seen_graphs -> int
            Returns the number of training graphs collected from `collect` calls.

        get_n_seen_nodes -> int
            Returns the number of training nodes collected from `collect` calls.

        get_n_seen_edges -> int
            Returns the number of training edges collected from `collect` calls.

        get_n_seen_initial_colours -> int
            Returns the number of initial colours collected from `collect` calls.

        get_n_seen_refined_colours -> int
            Returns the number of refined colours collected from `collect` calls.

        set_weights(self, weights: Union[list[float], list[int]]) -> None
            Set the weights to predict heuristics directly with this class. The weights must be a list of floats, integers or a numpy array of floats. The length of the weights must be the same as the number of features collected.

        get_weights(self) -> np.ndarray
            Return stored weights. Raises an error if weights do not exist.

        predict(self, state: list[Atom]) -> float
            Predict a heuristic value for a state. The state must be a list of atoms. The weights must be set with `set_weights` before calling this method.

        save(self, filename: str) -> None
            Save the feature generator to a json file with path `filename`. The directory to `filename` will be created automatically if it does not exist. The file will contain information about the generator configuration, the computed hash function, and information about features.

        load(filename: str) -> WLFeatures
            Load a feature generator from a json file with path `filename`. The file should have been created by `save`. The directory to `filename` will be created automatically if it does not exist.
    """

    def __init__(
        self,
        domain: Domain,
        graph_representation: Optional[str] = "ilg",
        iterations: int = 2,
        prune_features: Optional[str] = None,
        multiset_hash: bool = False,
        **kwargs,
    ) -> None:
        # Check if we want to load from a file. See WLFeatures.load
        if "filename" in kwargs:
            super().__init__(filename=kwargs["filename"])
            return

        choices = [None, "custom", "ilg"]
        if graph_representation not in choices:
            raise ValueError(f"graph_representation must be one of {choices}")
        if graph_representation is None:
            graph_representation = "custom"

        choices = [None, "no_prune", "collapse", "collapse_by_layer"]
        if prune_features not in choices:
            raise ValueError(f"prune_features must be one of {choices}")
        if prune_features is None:
            prune_features = "no_prune"

        super().__init__(
            domain=domain,
            graph_representation=graph_representation,
            iterations=iterations,
            prune_features=prune_features,
            multiset_hash=multiset_hash,
        )

    def set_weights(self, weights: Union[list[float], list[int]]) -> None:
        if not isinstance(weights, list):
            raise ValueError("Input weights must be a Python list.")
        super().set_weights(weights)

    def get_weights(self) -> list[float]:
        weights = super().get_weights()
        return weights
    
    def predict(self, state: list[Atom]) -> float:
        return super().predict(state)

    @staticmethod
    def load(filename: str) -> "WLFeatures":
        return WLFeatures(domain=None, filename=filename)

    def save(self, filename: str) -> None:
        filename_dir = os.path.dirname(filename)
        if len(filename_dir) > 0 and not os.path.exists(filename_dir):
            os.makedirs(filename_dir)
        super().save(filename)
