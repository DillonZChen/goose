import time
from itertools import product
from typing import Dict, Iterable, List, Tuple
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
    parser.add_argument(
        "--save_file",
        type=str,
        default=None,
        help="save file of model weights",
    )
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
    
    raise ValueError(f"Unknown model name: {model_name}")


def predict(
    model,
    X_train: np.array,
    y_train_true: Dict[str, np.array],
    X_val: np.array,
    y_val_true: Dict[str, np.array],
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


class BaseModel:
    def __init__(self, args, schemata=None) -> None:
        self.model_name = args.model
        self._args = args
        self._models = {}
        if schemata is None:
            schemata = [ALL_KEY]
        for schema in schemata:
            self._models[schema] = init_reg_model(
                model_name=self.model_name,
                regularise=(schema == ALL_KEY),
            )

    def train(self) -> None:
        """set train mode, similar to pytorch models"""
        pass

    def eval(self) -> None:
        """set eval mode, similar to pytorch models"""
        pass

    def fit_all(self, X, y_dict) -> None:
        # fit up to len(schemata) + 1 models
        for schema in self._models:
            self.fit(X, y_dict[schema], schema=schema)

    def fit(self, X, y, schema=ALL_KEY) -> None:
        assert schema in self._models
        print(f"Fitting model for learning number of '{schema}' in a plan...")
        if self.model_name == "mip":
            X = self._transform_for_mip(X)
        self._models[schema].fit(X, y)

    def _transform_for_mip(self, X):
        return X

    def predict_all(self, X, schema=ALL_KEY) -> Dict[str, np.array]:
        y_dict = {}
        for schema in self._models:
            y_dict[schema] = self.predict(X, schema=schema)
        return y_dict

    def predict(self, X, schema=ALL_KEY) -> np.array:
        ret = self._models[schema].predict(X)
        return ret

    def predict_with_std(self, X, schema=ALL_KEY) -> Tuple[np.array, np.array]:
        """for Bayesian models only"""
        return self._models[schema].predict(X, return_std=True)

    def get_learning_model(self, schema=ALL_KEY):
        return self._models[schema]

    def update_schemata_to_learn(
        self, schemata_to_learn: Iterable[str]
    ) -> None:
        initial_schemata = set(self._models.keys())
        schemata_to_keep = set(schemata_to_learn)
        for schema in initial_schemata:
            if schema not in schemata_to_keep:
                del self._models[schema]

    def get_weights(self) -> np.array:
        if self.model_name == "gpr":
            """
            For the general GP case:
                L = cholesky(k(X, X)) X is X_train
                a = L \ (L \ t); Ax = b => x = A \ b
                m(x) = k(x, X) . a
                v = L \ k(X, x)
                s^2(x) = k(x, x) - v^T . v
            but if we use dot product kernel, we can simplify and get
            """
            weights = np.sum(
                m.alpha_ @ m.X_train_ for m in self._models.values()
            )
        else:
            weights = np.sum(model.coef_ for model in self._models.values())
        return weights

    def get_bias(self) -> float:
        try:
            bias = np.sum(model.intercept_ for model in self._models.values())
            if type(bias) == float:
                return bias
            if type(bias) == np.float64:
                return float(bias)
            return float(bias[0])  # linear-svr returns array
        except Exception:
            return 0

    def get_num_weights(self):
        return len(self.get_weights())

    def get_num_zero_weights(self):
        return np.count_nonzero(self.get_weights() == 0)

    def debug_weights(self):
        print("Verbose linear weights information")
        weights = self.get_weights()
        model_hash = self.get_hash()
        # nonzero_weights = np.nonzero(weights)[0]
        # print(f"indices of nonzero weights:", nonzero_weights)
        for schema, model in self._models.items():
            print(schema)
            weights = model.coef_
            nonzero_weights = np.nonzero(weights)[0]
            set_nonzero_weights = set(nonzero_weights)
            for k, v in model_hash.items():
                if v in set_nonzero_weights:
                    print(f"{round(weights[v]):>5} * {k}")

    def setup_for_saving(self, save_file: str) -> None:
        pass

    def setup_after_loading(self, save_file: str) -> None:
        pass
