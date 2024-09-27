from argparse import Namespace

from sklearn.base import BaseEstimator
from sklearn.calibration import LinearSVC
from sklearn.gaussian_process import GaussianProcessClassifier, GaussianProcessRegressor
from sklearn.gaussian_process.kernels import DotProduct
from sklearn.svm import LinearSVR

from learner.feature_generation.estimator.brute_force import BruteForce
from learner.feature_generation.estimator.mip_cp import MipCp
from learner.feature_generation.estimator.mip_eq import MipEq
from learner.feature_generation.estimator.mip_rk import MipRk

linear_kernel = DotProduct(sigma_0=0, sigma_0_bounds="fixed")


def check_estimator_compatibility(opts: Namespace) -> None:
    estimator_name, target = opts.estimator_name, opts.target
    err_msg = f"{estimator_name} does not support {target}"
    if estimator_name in {"gpr", "svr", "mipeq", "bf"}:
        assert target == "h", err_msg
    elif estimator_name in {"miprk"}:
        assert target == "r", err_msg
    else:
        assert target in {"p", "d"}, err_msg


def get_estimator(opts) -> BaseEstimator:
    estimator_name = opts.estimator_name
    seed = opts.seed
    if estimator_name == "gpr":
        return GaussianProcessRegressor(kernel=linear_kernel, alpha=1e-7, random_state=seed)
    elif estimator_name == "gpc":
        return GaussianProcessClassifier(kernel=linear_kernel, random_state=seed)
    elif estimator_name == "svr":
        return LinearSVR(dual="auto", epsilon=0.5, C=1.0, fit_intercept=False, random_state=seed)
    elif estimator_name == "svc":
        return LinearSVC(dual="auto", C=1.0, fit_intercept=False, random_state=seed)
    elif estimator_name == "mipeq":
        return MipEq()
    elif estimator_name == "mipcp":
        return MipCp()
    elif estimator_name == "miprk":
        return MipRk()
    elif estimator_name == "bf":
        return BruteForce()
    else:
        raise ValueError(f"Unknown estimator: {estimator_name}")
