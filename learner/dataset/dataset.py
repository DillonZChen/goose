import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import random
from util.stats import get_stats
from torch_geometric.loader import DataLoader
from sklearn.model_selection import train_test_split
from dataset.graphs_gnn import get_graph_data as get_graph_data_gnn
from dataset.graphs_kernel import get_graph_data as get_graph_data_kernel
from dataset.transform import preprocess_data


def get_loaders_from_args_gnn(args):
  model_name = args.model
  batch_size = args.batch_size
  domain = args.domain
  rep = args.rep
  max_nodes = args.max_nodes
  cutoff = args.cutoff
  small_train = args.small_train
  num_workers = 0
  pin_memory = True

  dataset = get_graph_data_gnn(domain=domain, representation=rep)
  dataset = preprocess_data(model_name, data_list=dataset, c_hi=cutoff, n_hi=max_nodes, small_train=small_train)
  get_stats(dataset=dataset, desc="Whole dataset")

  trainset, valset = train_test_split(dataset, test_size=0.15, random_state=4550)

  get_stats(dataset=trainset, desc="Train set")
  get_stats(dataset=valset, desc="Val set")
  print("train size:", len(trainset))
  print("validation size:", len(valset))

  train_loader = DataLoader(trainset, batch_size=batch_size, shuffle=True, pin_memory=pin_memory, num_workers=num_workers)
  val_loader = DataLoader(valset, batch_size=batch_size, shuffle=False, pin_memory=pin_memory, num_workers=num_workers)

  return train_loader, val_loader

def get_dataset_from_args_kernels(args):
  rep = args.rep
  domain = args.domain

  dataset = get_graph_data_kernel(domain=domain, representation=rep)
  if args.small_train:
    dataset = random.sample(dataset, min(len(dataset, 1000)))
  get_stats(dataset=dataset, desc="Whole dataset")

  return dataset
