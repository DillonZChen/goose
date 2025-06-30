from typing import Optional

from learning.predictor.linear_model.linear_model import LinearModel
from learning.predictor.linear_model.rank_lp import LinearProgramRanker

from .gpr import GaussianProcessRegressor
from .lasso import LassoRegression
from .rank_gpc import GaussianProcessClassifierRanker
from .rank_mip import MixedIntegerProgramRanker
from .rank_svm import SupportVectorMachineRanker
from .svr import SupportVectorRegression

_LINEAR_MODELS = {
    "gpr": GaussianProcessRegressor,
    "svr": SupportVectorRegression,
    "lasso": LassoRegression,
    "rank-mip": MixedIntegerProgramRanker,
    "rank-lp": LinearProgramRanker,
    "rank-svm": SupportVectorMachineRanker,
    "rank-gpc": GaussianProcessClassifierRanker,
}


def get_available_predictors():
    return set(_LINEAR_MODELS.keys())


def is_rank_predictor(predictor_name: Optional[str]):
    if predictor_name is None:
        return False
    return _LINEAR_MODELS[predictor_name].IS_RANK


def get_predictor(predictor_name: str) -> LinearModel:
    if predictor_name in _LINEAR_MODELS:
        return _LINEAR_MODELS[predictor_name]()
    else:
        raise ValueError(f"Unknown model {predictor_name}")
