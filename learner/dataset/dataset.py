import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

from util.stats import get_stats
from torch_geometric.loader import DataLoader
from sklearn.model_selection import train_test_split
from dataset.graphs import get_graph_data
from representation.node_features import add_features
from util.transform import extract_testset_ipc, preprocess_data, sample_strategy


def get_loaders_from_args(args):

    model_name = args.model
    batch_size = args.batch_size
    domain = args.domain
    rep = args.rep
    max_nodes = args.max_nodes
    cutoff = args.cutoff
    small_train = args.small_train
    strategy = args.strategy
    num_workers = 0
    pin_memory = True

    dataset = get_graph_data(domain=domain, representation=rep)
    dataset = preprocess_data(model_name, data_list=dataset, c_hi=cutoff, n_hi=max_nodes, small_train=small_train)
    dataset = add_features(dataset, args)
    get_stats(dataset=dataset, desc="Whole dataset")

    trainset, valset = train_test_split(dataset, test_size=0.15, random_state=4550)

    trainset = sample_strategy(data_list=trainset, strategy=strategy)
    get_stats(dataset=trainset, desc="Train set")
    get_stats(dataset=valset, desc="Val set")
    print("train size:", len(trainset))
    print("validation size:", len(valset))

    train_loader = DataLoader(trainset, batch_size=batch_size, shuffle=True, pin_memory=pin_memory, num_workers=num_workers)
    val_loader = DataLoader(valset, batch_size=batch_size, shuffle=False, pin_memory=pin_memory, num_workers=num_workers)

    return train_loader, val_loader
