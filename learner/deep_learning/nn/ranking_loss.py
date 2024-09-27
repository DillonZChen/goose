from itertools import product

import torch


class RankingLoss:
    def __init__(self):
        self.cnt = 0
        pass

    def forward(self, h_pred, good_succ_idxs, maybe_bad_succ_idxs, def_bad_succ_idxs):
        ## h_pred consists of prediction for an opt plan state and its successors
        ## y_true contains the heuristic values if solved, lower bound if timed out,
        ## and inf if a deadend
        ## see also mip_rk.py

        ## if we want to ensure a - b >= 0
        ## then we want b - a <= 0
        ## so, loss = max(0, b - a)

        loss = 0
        for bad_succ_idxs, diff in [(maybe_bad_succ_idxs, 0), (def_bad_succ_idxs, 1)]:
            assert len(bad_succ_idxs) == len(good_succ_idxs)
            for good_succ_idx, bad_succ_idx in zip(good_succ_idxs, bad_succ_idxs):
                ## https://arxiv.org/abs/1608.01302
                ## bad = big, good = small
                ## we want  + h_bigger - h_smaller - 1 >= 0 
                ## i.e.     - h_bigger + h_smaller + 1 <= 0

                for bad_i in bad_succ_idx:
                    lhs = -h_pred[bad_i] + h_pred[good_succ_idx] + diff
                    loss += torch.sum(torch.maximum(torch.zeros_like(lhs), lhs))

                self.cnt += len(bad_succ_idx) * len(good_succ_idx)

                # print(len(good_succ_idx), len(bad_succ_idx))

        return loss
