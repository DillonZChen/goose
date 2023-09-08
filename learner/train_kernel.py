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
  parser.add_argument('-p', '--prune', type=int, default=0,
                      help="discard colours with total train count <= prune")
  
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
  
  parser.add_argument('-c', '--compactify', action='store_true',
                      help="compactify weights")
  
  parser.add_argument('--save-file', type=str, default=None,
                      help="save file of model weights")
  parser.add_argument('--small-train', action="store_true", 
                      help="use small train set, useful for debugging")
  
  return parser.parse_args()


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

  print(f"Training on entire {args.domain} for {args.model}...")
  t = time.time()
  model.fit(X, y)
  print(f"Model training completed in {time.time()-t:.2f}s")
  y_pred = model.predict(X)
  for metric in _SCORING:
    score = _SCORING[metric](model.get_learning_model(), X, y)
    print(f"train_{metric}: {score:.2f}")

  if args.compactify:
    lb = 0
    ub = 1
    weights = model.get_weights()
    bias = model.get_bias()
    y_pred_int = np.rint(y_pred)
    while ub - lb > 1e-5:
      cutoff = (lb + ub) / 2
      indices = (np.abs(weights) > cutoff)
      pruned_weights = weights[indices]

      if len(pruned_weights) == 0:
        ub /= 2
        continue
      
      X_pruned = X[:,indices]
      y_pruned = X[:,indices] @ pruned_weights + bias
      y_pruned_int = np.rint(y_pruned)

      diff_int = np.linalg.norm(y_pred_int - y_pruned_int)

      if diff_int == 0:
        lb = cutoff
      else:
        ub = cutoff
    
    print(f"pruned weights with cutoff {cutoff}")
    indices = (np.abs(weights) > cutoff)
    model.set_weight_indices(indices)

  save_kernel_model(model, args)
  try:
    print(f"zero_weights: {model.get_num_zero_weights()}/{model.get_num_weights()} = " + \
          f"{model.get_num_zero_weights()/model.get_num_weights():.2f}")
  except Exception as e:
    pass
