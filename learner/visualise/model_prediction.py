import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import time
import torch
import argparse
import torch_geometric
from sklearn.model_selection import train_test_split
from torch_geometric.loader import DataLoader

from gen_data import get_graph_data
from models import *
from planning.strips import get_strips_problem
from util.save_load import load_model
from representation import add_features
from util.stats import visualise_train_stats, get_stats
from util.transform import preprocess_data


def main():
  parser = argparse.ArgumentParser()

  parser.add_argument('-m', type=str, help='path to model params file', required=True)
  parser.add_argument('--n_lo', type=int, help="lower bound for number of nodes (-1 for no bound)", required=True)
  parser.add_argument('--n_hi', type=int, help="upper bound for number of nodes (-1 for no bound)", required=True)
  parser.add_argument('--c_lo', type=int, help="lower bound for cost (-1 for no bound)", required=True)
  parser.add_argument('--c_hi', type=int, help="upper bound for cost (-1 for no bound)", required=True)
  parser.add_argument('--device', type=int, default=0)
  parser.add_argument('--view_cm', action="store_true")

  main_args = parser.parse_args()

  model_path = main_args.m
  n_lo = main_args.n_lo
  n_hi = main_args.n_hi
  c_lo = main_args.c_lo
  c_hi = main_args.c_hi

  # cuda
  device = torch.device(f'cuda:{main_args.device}' if torch.cuda.is_available() else 'cpu')

  model, args = load_model(model_path, print_args=False)
  model = model.to(device)

  dataset = get_graph_data(representation=args.rep, task=args.task, parser=args.parser)
  dataset = preprocess_data(model_name=args.model,
                            data_list=dataset,
                            task=args.task,
                            n_lo=n_lo,
                            n_hi=n_hi,
                            c_lo=c_lo,
                            c_hi=c_hi)
  dataset = add_features(dataset, args)
  get_stats(dataset, task=args.task)

  bs = args.batch_size
  if 50000 < n_hi or n_hi == -1:
    bs = 1
  elif 10000 < n_hi <= 50000:
    bs = 2
  train_loader = DataLoader(dataset, batch_size=bs, shuffle=False)
  val_loader = None
  cm_title = f"cm_test"
  if n_hi < 0:
    n_hi = max([data.x.shape[0] for data in dataset])
  if c_hi < 0:
    c_hi = max([int(round(data.y)) for data in dataset])
  n_lo = max(0, n_lo)
  c_lo = max(0, c_lo)
  n_suffix = f"_N{n_lo}-{n_hi}"
  c_suffix = f"_C{c_lo}-{c_hi}"
  cm_title = cm_title + c_suffix + n_suffix
  visualise_train_stats(model, device, train_loader, val_loader, view_cm=main_args.view_cm, cm_train=cm_title)

if __name__ == "__main__":
  main()
