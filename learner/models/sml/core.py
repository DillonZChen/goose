from sklearn.metrics import mean_squared_error
from sklearn.gaussian_process import GaussianProcessRegressor
from sklearn.model_selection import train_test_split
from sklearn.neural_network import MLPRegressor
from sklearn.linear_model import BayesianRidge, Lasso, Ridge, LinearRegression
from sklearn.svm import LinearSVR, SVR
from sklearn.gaussian_process.kernels import DotProduct
from .mip import MIP


FREQUENTIST_MODELS = {
    "mip",
    #
    "linear-regression",
    "ridge",
    "lasso",
    #
    "linear-svr",
    "quadratic-svr",
    "cubic-svr",
    "rbf-svr",
    #
    "mlp",
}

BAYESIAN_MODELS = {
    "blr",  # bayesian linear regression
    "gpr",  # gaussian process with dot product kernel
}

MODELS = FREQUENTIST_MODELS.union(BAYESIAN_MODELS)

LINEAR_MODELS = {
    "gpr",
    "linear-svr",
    "linear-regression",
    "ridge",
    "lasso",
    "mip",
}


def init_reg_model(model_name: str, regularise: bool):
    if regularise:
        e = 0
        c = 1e5  # inverse strength
        a = 0
    else:
        e = 0.5
        a = 1.0
        c = 1.0

    if model_name == "empty":
        return None
    if model_name == "mip":
        return MIP()
    if model_name == "linear-regression":
        return LinearRegression(fit_intercept=False)
    if model_name == "linear-svr":
        return LinearSVR(dual="auto", epsilon=e, C=c, fit_intercept=False)
    if model_name == "lasso":
        return Lasso(alpha=a)
    if model_name == "ridge":
        return Ridge(alpha=a)
    if model_name == "rbf-svr":
        return SVR(kernel="rbf", epsilon=e, C=c)
    if model_name == "quadratic-svr":
        return SVR(kernel="poly", degree=2, epsilon=e, C=c)
    if model_name == "cubic-svr":
        return SVR(kernel="poly", degree=3, epsilon=e, C=c)
    if model_name == "mlp":
        return MLPRegressor(
            hidden_layer_sizes=(64,),
            batch_size=16,
            learning_rate="adaptive",
            early_stopping=True,
            validation_fraction=0.15,
        )
    if model_name == "blr":
        return BayesianRidge()
    if model_name == "gpr":
        return GaussianProcessRegressor(kernel=DotProduct(), alpha=1e-7)
