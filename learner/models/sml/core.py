import time
from itertools import product
from typing import Dict, List
import numpy as np
from sklearn.metrics import mean_squared_error
from sklearn.gaussian_process import GaussianProcessRegressor
from sklearn.model_selection import train_test_split
from sklearn.neural_network import MLPRegressor
from sklearn.linear_model import BayesianRidge, Lasso, Ridge, LinearRegression
from sklearn.svm import LinearSVR, SVR
from sklearn.gaussian_process.kernels import DotProduct
from sklearn.metrics import log_loss, mean_squared_error
from util.metrics import f1_macro
from .mip import MIP
from .schema_count_strategy import (
    ALL_KEY,
    SCS_NONE,
    SCS_ALL,
    SCS_SCHEMA_APPROX,
    SCS_SCHEMA_EXACT,
)


F1_KEY = "f1_macro"
MSE_KEY = "mse"

F1_KEEP_TOL = 1e-3

SCORER = {
    MSE_KEY: mean_squared_error,
    F1_KEY: f1_macro,
}

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


def add_sml_args(parser):
    parser.add_argument("--seed", type=int, default=0, help="random seed")
    parser.add_argument(
        "-m",
        "--model",
        type=str,
        default="linear-svr",
        choices=MODELS,
        help="ML model",
    )
    parser.add_argument(
        "--save-file",
        type=str,
        default=None,
        help="save file of model weights",
    )
    parser.add_argument(
        "-s",
        "--schema_count_strategy",
        default=SCS_NONE,
        choices=[SCS_NONE, SCS_ALL, SCS_SCHEMA_EXACT, SCS_SCHEMA_APPROX],
        help="Strategy for learning schema counts.\n"
        + f"{SCS_NONE}: learn h* prediction.\n"
        + f"{SCS_ALL}: learn schema counts and sum with h* prediction.\n"
        + f"{SCS_SCHEMA_EXACT}: try to learn schema counts exactly.\n"
        + f"{SCS_SCHEMA_APPROX}: try to learn schema counts approximately.\n",
    )
    return parser


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


def predict(
    model,
    X_train: np.array,
    y_train_true: Dict[np.array],
    X_val: np.array,
    y_val_true: Dict[np.array],
    schemata: List[str],
    schema_strat: List[str],
):
    # predict on train and val sets
    print("Predicting...")
    t = time.time()
    print("  For train...")
    y_train_pred = model.predict_all(X_train)
    print("  For val...")
    y_val_pred = model.predict_all(X_val)
    print(f"Predicting completed in {time.time()-t:.2f}s")

    # metrics
    print("Scores on prediction against h*:")
    itrs = list(product(SCORER.keys(), schemata))
    train_scores = {
        (m, s): SCORER[m](y_train_true[s], y_train_pred[s]) for m, s in itrs
    }
    val_scores = {
        (m, s): SCORER[m](y_val_true[s], y_val_pred[s]) for m, s in itrs
    }
    t = time.time()
    schemata_to_keep = set()
    print(f"{'':<10} {'schema':<20} {'train':<10} {'val':<10}")
    for metric in SCORER:
        for schema in schemata:
            t = train_scores[(metric, schema)]
            v = val_scores[(metric, schema)]
            print(f"{metric:<10} {schema:<20} {t:<10.4f} {v:<10.4f}")
            if (
                abs(v - 1) < F1_KEEP_TOL and metric == F1_KEY
            ) or schema_strat == SCS_SCHEMA_APPROX:
                schemata_to_keep.add(schema)
    if schema_strat in {SCS_ALL, SCS_NONE}:
        schemata_to_keep.add(ALL_KEY)

    # log schemata to learn
    print(f"{len(schemata_to_keep)} schemata to keep")
    for s in sorted(schemata_to_keep):
        print(f"  {s}")
    model.update_schemata_to_learn(schemata_to_keep)

    # print % weights that are zero
    try:
        n_zero_weights = model.get_num_zero_weights()
        n_weights = model.get_num_weights()
        print(
            f"zero_weights: {n_zero_weights}/{n_weights} = {n_zero_weights/n_weights:.2f}"
        )
    except Exception as e:  # not possible for nonlinear kernel methods
        pass
