from argparse import Namespace
from typing import Dict, List, Tuple

import numpy as np
from tqdm import tqdm

from learner.dataset.dataset import ALL_KEY, Dataset
from learner.dataset.ranking_data import RankingData
from learner.problem.numeric_state import NumericState
from learner.representation import Representation
from util.timer import TimerContextManager


class RankingDataset(Dataset):
    def __init__(self, opts: Namespace, representation: Representation) -> None:
        super().__init__(opts, representation)

        if opts.schemata_strategy != "all":
            raise ValueError
        if opts.val_ratio > 0:
            raise ValueError

        self._schemata_keys = {ALL_KEY}

        with TimerContextManager("converting data to model inputs"):
            raw_dataset = self._raw_dataset

            ranking_data: List[RankingData] = []
            for problem, state_datas in tqdm(raw_dataset.items()):
                state_to_h: Dict[NumericState, float] = {}
                state_to_desc: Dict[NumericState, str] = {}
                edges: Dict[NumericState, List[NumericState]] = {}  # parent to child
                for state_data in state_datas:
                    cur_state = state_data.state
                    if cur_state not in state_to_h:
                        state_to_h[cur_state] = state_data.heuristic
                        state_to_desc[cur_state] = state_data.description
                    else:
                        # print(cur_state)
                        # print(state_to_desc[cur_state], state_data.description)
                        # print(state_to_h[cur_state], state_data.heuristic)
                        # assert state_to_desc[cur_state] == state_data.description
                        # assert state_to_h[cur_state] == state_data.heuristic
                        pass  ## TODO fix this

                    par_state = state_data.parent_state
                    if par_state not in edges and par_state is not None:
                        edges[par_state] = []
                    if par_state is not None:
                        edges[par_state].append(cur_state)

                for parent, children in edges.items():
                    states = []
                    good_idxs = []
                    maybe_bad_idxs = []
                    def_bad_idxs = []

                    idx = len(states)
                    states.append(parent)
                    def_bad_idxs.append(idx)

                    best_h = float('inf')
                    for child in children:
                        if state_to_desc[child] == "opt":
                            best_h = min(best_h, state_to_h[child])
                    ## TODO fix this
                    # assert best_h != float('inf')
                    if best_h == float('inf'):
                        continue
                    for child in children:
                        desc = state_to_desc[child]
                        h = state_to_h[child]

                        idx = len(states)
                        states.append(child)
                        if h == best_h and desc == "opt":
                            good_idxs.append(idx)
                        elif desc == "dead" or h > best_h:
                            def_bad_idxs.append(idx)
                        else:
                            maybe_bad_idxs.append(idx)

                    ## there may be duplicate graphs due to different parents
                    r_data = RankingData(
                        problem=problem,
                        states=states,
                        good_idxs=good_idxs,
                        maybe_bad_idxs=maybe_bad_idxs,
                        def_bad_idxs=def_bad_idxs,
                    )
                    ranking_data.append(r_data)

            ret = self.representation.transform_ranking_dataset(ranking_data)
            ## succ_groups is set for wlf but not gnn
            self.dataset, self.succ_groups = ret

        if len(self.dataset) == 0:
            print("Training data is empty! Terminating.")
            exit(0)

    def get_dataset_split(self):
        # cannot convert to array yet, bunch of dicts
        _y_true = [d[1] for d in self.dataset]
        _X = np.array([d[0] for d in self.dataset])

        assert self._opts.val_ratio == 0
        print("X shape:", _X.shape)
        X_tr, X_val, y_tr_true, y_va_true = _X, [], _y_true, []

        # reshape y from array of dicts to dict of arrays
        y_tr_true = {k: np.array([d[k] for d in y_tr_true]) for k in self.schemata_keys}
        y_va_true = {k: np.array([d[k] for d in y_va_true]) for k in self.schemata_keys}

        return X_tr, X_val, y_tr_true, y_va_true

    def get_metrics(self) -> Dict[str, callable]:
        return {}
