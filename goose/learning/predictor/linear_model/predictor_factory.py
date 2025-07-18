from typing import Optional

from goose.learning.predictor.linear_model.linear_model import LinearModel
from goose.learning.predictor.linear_model.linear_models.gpc import GaussianProcessClassifier
from goose.learning.predictor.linear_model.linear_models.gpr import GaussianProcessRegressor
from goose.learning.predictor.linear_model.linear_models.lasso import LassoRegression
from goose.learning.predictor.linear_model.linear_models.rank_gpc import GaussianProcessClassifierRanker
from goose.learning.predictor.linear_model.linear_models.rank_lp import LinearProgramRanker
from goose.learning.predictor.linear_model.linear_models.rank_mip import MixedIntegerProgramRanker
from goose.learning.predictor.linear_model.linear_models.rank_svm import SupportVectorMachineRanker
from goose.learning.predictor.linear_model.linear_models.svm import SupportVectorMachine
from goose.learning.predictor.linear_model.linear_models.svr import SupportVectorRegression
from goose.learning.predictor.linear_model.unitary_classifier import UnitaryClassifier


_LINEAR_MODELS = {
    "gpr": GaussianProcessRegressor,
    "svr": SupportVectorRegression,
    "lasso": LassoRegression,
    "rank-mip": MixedIntegerProgramRanker,
    "rank-lp": LinearProgramRanker,
    "rank-svm": SupportVectorMachineRanker,
    "rank-gpc": GaussianProcessClassifierRanker,
    "gpc": GaussianProcessClassifier,
    "svm": SupportVectorMachine,
}


def get_available_predictors() -> set[str]:
    return set(_LINEAR_MODELS.keys())


def is_rank_predictor(predictor_name: str) -> bool:
    return _LINEAR_MODELS[predictor_name].IS_RANK


def is_unitary_classifier(predictor_name: str) -> bool:
    return issubclass(_LINEAR_MODELS[predictor_name], UnitaryClassifier)


def get_predictor(predictor_name: str) -> LinearModel:
    if predictor_name in _LINEAR_MODELS:
        return _LINEAR_MODELS[predictor_name]()
    else:
        raise ValueError(f"Unknown model {predictor_name}")
