from argparse import Namespace

from torch.nn import BCELoss, MSELoss

from learner.deep_learning.nn.ranking_loss import RankingLoss


def get_loss_function(opts: Namespace):
    target = opts.target
    if target == "h":
        return MSELoss()
    elif target in {"p", "d"}:
        # BCELogits better but more annoying in code structure to handle
        return BCELoss()
    elif target == "r":
        return RankingLoss()
    else:
        raise ValueError(f"Unknown target {target}")
