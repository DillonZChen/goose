from learning.predictor.rank_lp import LinearProgramRanker

from .gpr import GaussianProcessRegressor
from .rank_mip import MixedIntegerProgramRanker
from .rank_svm import SVMRanker
from .svr import SupportVectorRegression
from .rank_gpc import GaussianProcessRanker


def get_predictor(predictor_name: str):
    if predictor_name == "gpr":
        return GaussianProcessRegressor()
    elif predictor_name == "svr":
        return SupportVectorRegression()
    elif predictor_name == "rank-mip":
        return MixedIntegerProgramRanker()
    elif predictor_name == "rank-lp":
        return LinearProgramRanker()
    elif predictor_name == "rank-svm":
        return SVMRanker()
    elif predictor_name == "rank-gpc":
        return GaussianProcessRanker()
    else:
        raise ValueError(f"Unknown model {predictor_name}")
