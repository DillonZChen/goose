import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

from util.stats import get_stats
from torch_geometric.loader import DataLoader
from sklearn.model_selection import train_test_split
from dataset.graphs import get_graph_data
from representation.node_features import add_features
from util.transform import extract_testset_ipc, preprocess_data, sample_strategy


def get_loaders_from_args(args, adapt=False):

    model_name = args.model
    batch_size = args.batch_size
    domain = args.domain
    rep = args.rep
    max_nodes = args.max_nodes
    cutoff = args.cutoff
    small_train = args.small_train
    strategy = args.strategy
    val = args.val
    test = args.test
    heuristic=args.heuristic
    tar_domain=adapt
    # num_workers=0 if adapt else 4
    # pin_memory=True
    num_workers = 0
    pin_memory = True
    if tar_domain and "-only" not in tar_domain:
       tar_domain = tar_domain + "-only"

    if not adapt:
      dataset = get_graph_data(domain=domain, representation=rep)
      dataset = preprocess_data(model_name, data_list=dataset, heuristic=heuristic, c_hi=cutoff, n_hi=max_nodes, small_train=small_train)
      dataset = add_features(dataset, args)
      get_stats(dataset=dataset, desc="Whole dataset")

      if test:
          trainvalset, testset = train_test_split(dataset, test_size=0.2, random_state=4550)
          trainset, valset = train_test_split(trainvalset, test_size=0.25, random_state=4550)
      else:
          trainvalset, testset = dataset, []
          trainset, valset = train_test_split(trainvalset, test_size=0.15, random_state=4550)

      trainset = sample_strategy(data_list=trainset, strategy=strategy)
      get_stats(dataset=trainset, desc="Train set")
      get_stats(dataset=valset, desc="Val set")
      get_stats(dataset=testset, desc=f"Test set")
      print("train size:", len(trainset))
      print("validation size:", len(valset))
      print("test size:", len(testset))

      train_loader = DataLoader(trainset, batch_size=batch_size, shuffle=True, pin_memory=pin_memory, num_workers=num_workers)
      val_loader = DataLoader(valset, batch_size=batch_size, shuffle=False, pin_memory=pin_memory, num_workers=num_workers)
      test_loader = DataLoader(testset, batch_size=batch_size, shuffle=False, pin_memory=pin_memory, num_workers=num_workers)

      return train_loader, val_loader, test_loader
    else:
      trainvalset = get_graph_data(domain=domain, representation=rep, task=task)
      trainvalset = preprocess_data(model_name, data_list=trainvalset, heuristic=heuristic, c_hi=cutoff, n_hi=max_nodes, small_train=small_train)
      trainvalset = add_features(trainvalset, args)
      get_stats(dataset=trainvalset, desc="Whole trainvalset")
      trainset, valset = train_test_split(trainvalset, test_size=0.15, random_state=4550)
      get_stats(dataset=trainset, desc="Train set")
      get_stats(dataset=valset, desc="Val set")
      print("train size:", len(trainset))
      print("validation size:", len(valset))

      tarset = get_graph_data(domain=tar_domain, representation=rep, task=task)
      tarset = preprocess_data(model_name, data_list=tarset, heuristic=heuristic, c_hi=cutoff, n_hi=max_nodes, small_train=False)
      tarset = add_features(tarset, args)
      get_stats(dataset=tarset, desc="Tar set")
      print("target domain size:", len(tarset))

      src_loader = DataLoader(trainset, batch_size=batch_size, shuffle=True, pin_memory=pin_memory, num_workers=num_workers)
      tar_loader = DataLoader(tarset, batch_size=batch_size, shuffle=True, pin_memory=pin_memory, num_workers=num_workers)
      val_loader = DataLoader(valset, batch_size=batch_size, shuffle=False, pin_memory=pin_memory, num_workers=num_workers)

      return src_loader, tar_loader, val_loader
    