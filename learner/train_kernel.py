""" Main training pipeline script. """

import time
import argparse
import representation
import kernels
import numpy as np
from dataset.dataset import get_dataset_from_args_kernels
from util.save_load import print_arguments
from util.metrics import f1_macro
from sklearn.svm import LinearSVR, SVR
from sklearn.model_selection import cross_validate
from sklearn.metrics import make_scorer, f1_score, mean_squared_error


_MODELS = [
  "linear-svr",
  "svr",
]

_CV_FOLDS = 5
_MAX_MODEL_ITER = 1000

def create_parser():
  parser = argparse.ArgumentParser()

  parser.add_argument('-r', '--rep', type=str, required=True, choices=representation.REPRESENTATIONS,
                      help="graph representation to use")
  parser.add_argument('-k', '--kernel', type=str, required=True, choices=kernels.KERNELS,
                      help="graph representation to use")
  parser.add_argument('-l', '--iterations', type=int, default=5,
                      help="number of iterations for kernel algorithms")
  
  parser.add_argument('-m', '--model', type=str, default="linear-svr", choices=_MODELS,
                      help="ML model")
  parser.add_argument('-C', type=float, default=1,
                      help="regularisation parameter of SVR; strength is inversely proportional to C")
  parser.add_argument('-e', type=float, default=0.1,
                      help="epsilon parameter in epsilon insensitive loss function of SVR")
  
  parser.add_argument('-d', '--domain', type=str, default="goose-di",
                      help="domain to train on; defaults to goose-di which is di training")
  
  parser.add_argument('-s', '--seed', type=int, default=0,
                      help="random seed")
  
  parser.add_argument('--small-train', action="store_true", 
                      help="use small train set, useful for debugging")
  parser.add_argument('--save-file', dest="save_file", type=str, default=None,
                      help="file to save model weights")
  
  return parser

if __name__ == "__main__":
  parser = create_parser()
  args = parser.parse_args()
  print_arguments(args)

  np.random.seed(args.seed)

  graphs, y = get_dataset_from_args_kernels(args)
  kernel = kernels.KERNELS[args.kernel](args.iterations)
  kernel.read_train_data(graphs)

  print(f"Setting up training data and initialising model...")
  model_name = args.model
  t = time.time()

  kwargs = {
    "epsilon": args.e,
    "C": args.C,
    "max_iter": _MAX_MODEL_ITER,
  }
  if model_name == "linear-svr":
    model = LinearSVR(**kwargs)
    X = kernel.get_x(graphs)
  elif model_name == "svr":
    model = SVR(kernel="precomputed", **kwargs)
    X = kernel.get_k(graphs)
  else:
    raise NotImplementedError
  print(f"Set up training data in {time.time()-t:.2f}s")

  print(f"Performing {_CV_FOLDS}-fold cross validation on {model_name}...")
  scoring = {
    "mse": make_scorer(mean_squared_error),
    "f1_macro": make_scorer(f1_macro)
  }
  scores = cross_validate(model, X, y, cv=_CV_FOLDS, scoring=scoring, return_train_score=True)
  print(f"CV completed in {scores['fit_time'].sum()+scores['score_time'].sum():.2f}s")
  
  for metric in scoring:
    train_key = f"train_{metric}"
    test_key = f"test_{metric}"
    print(f"train_{metric}: {scores[train_key].mean():.2f} ± {scores[train_key].std():.2f}")
    print(f"test_{metric}: {scores[test_key].mean():.2f} ± {scores[test_key].std():.2f}")




