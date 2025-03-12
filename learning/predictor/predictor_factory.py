from learning.predictor.rank_lp import LinearProgramRanker

from .gpr import GaussianProcessRegressor
from .rank_gpc import GaussianProcessRanker
from .rank_mip import MixedIntegerProgramRanker
from .rank_svm import SVMRanker
from .svr import SupportVectorRegression

_PREDICTORS = {
    "gpr": GaussianProcessRegressor,
    "svr": SupportVectorRegression,
    "rank-mip": MixedIntegerProgramRanker,
    "rank-lp": LinearProgramRanker,
    "rank-svm": SVMRanker,
    "rank-gpc": GaussianProcessRanker,
}


def get_available_predictors():
    return set(_PREDICTORS.keys())


def is_rank_predictor(predictor_name: str):
    return _PREDICTORS[predictor_name].IS_RANK


def get_predictor(predictor_name: str):
    if predictor_name in _PREDICTORS:
        return _PREDICTORS[predictor_name]()
    else:
        raise ValueError(f"Unknown model {predictor_name}")
