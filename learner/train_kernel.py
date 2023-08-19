""" Main training pipeline script. """

import time
import argparse
import representation
import kernels
import numpy as np
from dataset.dataset import get_dataset_from_args_kernels
from util.save_load import print_arguments
from sklearn.svm import LinearSVR, SVR
from sklearn.model_selection import train_test_split
from sklearn.metrics import f1_score


_MODELS = [
  "linear-svr",
  "svr",
]

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

  graphs, y = get_dataset_from_args_kernels(args)
  kernel = kernels.KERNELS[args.kernel](args.iterations)
  kernel.read_train_data(graphs)

  graphs_train, graphs_val, y_train, y_val = train_test_split(graphs, y, test_size=0.25, random_state=args.seed)

  print(f"Setting up training data...")
  model_name = args.model
  t = time.time()
  if model_name == "linear-svr":
    model = LinearSVR()
    X_train = kernel.get_x(graphs_train)
    X_val = kernel.get_x(graphs_val)
  elif model_name == "svr":
    model = SVR(kernel="precomputed")
    X_train = kernel.get_k(graphs_train)
    X_val = kernel.get_k(graphs_val)
  else:
    raise NotImplementedError
  print(f"Set up training data in {time.time()-t:.2f}s")

  print(f"Fitting {model_name}...")
  t = time.time()
  model.fit(X_train, y_train)
  print(f"Model fitted in {time.time()-t:.2f}s")

  pred_train = np.rint(model.predict(X_train)).astype(int)
  pred_val = np.rint(model.predict(X_val)).astype(int)

  print(f"Train model score={model.score(X_train, y_train):.2f}")
  print(f"Val model score={model.score(X_val, y_val):.2f}")

  print(f"Train f1={f1_score(y_train, pred_train, average='macro'):.2f}")
  print(f"Val f1={f1_score(y_val, pred_val, average='macro'):.2f}")

  print(np.abs(y_train-pred_train))
  print(np.abs(y_val-pred_val))




