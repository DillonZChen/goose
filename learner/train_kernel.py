""" Main training pipeline script. """

import os
import time
import argparse
import numpy as np
import representation
import kernels
from sklearn.model_selection import cross_validate
from sklearn.metrics import make_scorer, mean_squared_error
from kernels.wrapper import MODELS
from dataset.dataset import get_dataset_from_args_kernels
from util.save_load import print_arguments, save_kernel_model
from util.metrics import f1_macro
from util.visualise import get_confusion_matrix

import warnings
warnings.filterwarnings('ignore') 

_CV_FOLDS = 5
_PLOT_DIR = "plots"
_SCORING = {
  "mse": make_scorer(mean_squared_error),
  "f1_macro": make_scorer(f1_macro)
}


def parse_args():
  parser = argparse.ArgumentParser()

  parser.add_argument('-r', '--rep', type=str, required=True, choices=representation.REPRESENTATIONS,
                      help="graph representation to use")
  
  parser.add_argument('-k', '--kernel', type=str, required=True, choices=kernels.KERNELS,
                      help="graph representation to use")
  parser.add_argument('-l', '--iterations', type=int, default=5,
                      help="number of iterations for kernel algorithms")
  
  parser.add_argument('-m', '--model', type=str, default="linear-svr", choices=MODELS,
                      help="ML model")
  parser.add_argument('-a', type=float, default=1,
                      help="L1 and L2 regularisation parameter of linear regression; strength is proportional to a")
  parser.add_argument('-C', type=float, default=1,
                      help="regularisation parameter of SVR; strength is inversely proportional to C")
  parser.add_argument('-e', type=float, default=0.1,
                      help="epsilon parameter in epsilon insensitive loss function of SVR")
  
  parser.add_argument('-d', '--domain', type=str, default="goose-di",
                      help="domain to train on; defaults to goose-di which is di training")
  
  parser.add_argument('-s', '--seed', type=int, default=0,
                      help="random seed")
  
  parser.add_argument('--cross-validation', action='store_true',
                      help="performs cross validation scoring; otherwise train on whole dataset")
  parser.add_argument('--save-file', type=str, default=None,
                      help="save file of model weights when not using --cross-validation")
  parser.add_argument('--visualise', action='store_true',
                      help="visualise train and test predictions; only used with --cross-validation")
  parser.add_argument('--small-train', action="store_true", 
                      help="use small train set, useful for debugging")
  
  return parser.parse_args()

def perform_training(X, y, model, args):
  print(f"Training on entire {args.domain} for {args.model}...")
  t = time.time()
  model.fit(X, y)
  print(f"Model training completed in {time.time()-t:.2f}s")
  for metric in _SCORING:
    print(f"train_{metric}: {_SCORING[metric](model.get_learning_model(), X, y):.2f}")
  print(f"zero_weights: {model.get_num_zero_weights()}/{model.get_num_weights()} = " + \
        f"{model.get_num_zero_weights()/model.get_num_weights():.2f}")
  save_kernel_model(model, args)
  return

def perform_cross_validation(X, y, model, args):
  print(f"Performing {_CV_FOLDS}-fold cross validation on {args.model}...")
  t = time.time()
  scores = cross_validate(
    model.get_learning_model(), X, y, 
    cv=_CV_FOLDS, scoring=_SCORING, return_train_score=True, n_jobs=-1,
    return_estimator=args.visualise, return_indices=args.visualise,
  )
  print(f"CV completed in {time.time() - t:.2f}s")
  
  for metric in _SCORING:
    train_key = f"train_{metric}"
    test_key = f"test_{metric}"
    print(f"train_{metric}: {scores[train_key].mean():.2f} ± {scores[train_key].std():.2f}")
    print(f"test_{metric}: {scores[test_key].mean():.2f} ± {scores[test_key].std():.2f}")

  if args.visualise:
    """ Visualise predictions and save to file
        Performs some redundant computations
    """

    if args.model == "svr":  # kernel matrix case
      raise NotImplementedError
    
    print("Saving visualisation...")
    train_trues = []
    train_preds = []
    test_trues = []
    test_preds = []

    for i in range(_CV_FOLDS):
      estimator = scores["estimator"][i]
      train_indices = scores["indices"]["train"][i]
      test_indices = scores["indices"]["test"][i]
      X_train = X[train_indices]
      X_test = X[test_indices]
      y_train = y[train_indices]
      y_test = y[test_indices]
      train_pred = estimator.predict(X_train)
      test_pred = estimator.predict(X_test)
      train_trues.append(y_train)
      train_preds.append(train_pred)
      test_trues.append(y_test)
      test_preds.append(test_pred)

    y_true_train = np.concatenate(train_trues)
    y_pred_train = np.concatenate(train_preds)
    y_true_test = np.concatenate(test_trues)
    y_pred_test = np.concatenate(test_preds)

    plt = get_confusion_matrix(y_true_train, y_pred_train, y_true_test, y_pred_test)

    os.makedirs(_PLOT_DIR, exist_ok=True)
    file_name = _PLOT_DIR + "/" + "_".join([args.domain, args.rep, args.kernel, str(args.iterations)]) + ".pdf"
    plt.savefig(file_name, bbox_inches="tight")
    print(f"Visualisation saved at {file_name}")
  return

if __name__ == "__main__":
  args = parse_args()
  print_arguments(args)

  np.random.seed(args.seed)

  graphs, y = get_dataset_from_args_kernels(args)

  print(f"Setting up training data and initialising model...")
  t = time.time()
  model = kernels.KernelModelWrapper(args)
  model.train()
  t = time.time()
  train_histograms = model.compute_histograms(graphs)
  print(f"Initialised WL for {len(graphs)} graphs in {time.time() - t:.2f}s")
  print(f"Collected {model.n_colours_} colours over {sum(len(G.nodes) for G in graphs)} nodes")
  X = model.get_matrix_representation(graphs, train_histograms)
  print(f"Set up training data in {time.time()-t:.2f}s")

  if args.cross_validation:
    perform_cross_validation(X, y, model, args)
  else:
    perform_training(X, y, model, args)
