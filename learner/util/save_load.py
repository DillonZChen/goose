""" Module for dealing with model saving and loading. """
import os
import torch
import joblib
import datetime
import representation
from argparse import Namespace as Args
from typing import Tuple


def arg_to_params(args, in_feat=4, out_feat=1):
    # this is an artifact of legacy code, it could
    nlayers = args.nlayers
    nhid = args.nhid
    in_feat = args.in_feat
    # out_feat = args.out_feat
    n_edge_labels = args.n_edge_labels
    pool = args.pool
    aggr = args.aggr
    rep = args.rep
    model_params = {
        "in_feat": in_feat,
        "out_feat": out_feat,
        "nlayers": nlayers,
        "n_edge_labels": n_edge_labels,
        "nhid": nhid,
        "aggr": aggr,
        "pool": pool,
        "rep": rep,
    }
    return model_params


def print_arguments(args, ignore_params=set()):
    print("Parsed arguments:")
    for k, v in vars(args).items():
        if k in ignore_params.union(
            {"device", "optimal", "save_model", "save_file", "no_tqdm", "tqdm", "fast_train"}
        ):
            continue
        print("{0:20}  {1}".format(k, v))
    print("___")


def save_model_from_dict(model_dict, args):
    if not hasattr(args, "save_file") or args.save_file is None:
        return
    save_file = args.save_file
    save_dir = os.path.dirname(save_file)
    if len(save_dir) > 0:
        os.makedirs(save_dir, exist_ok=True)
    print(f"Saving model at {save_file}...")
    torch.save((model_dict, args), save_file)
    print("Model saved!")
    print("Model parameter file:", save_file)
    return


def save_model(model, args):
    save_model_from_dict(model.model.state_dict(), args)
    return


def load_model(path, print_args=False):
    # returns (GNN, Args)
    import torch
    from gnns.gnn import Model

    print(f"Loading model from {path}...")
    if not os.path.exists(path):
        print(f"Model not found at {path}")
        exit(-1)
    if torch.cuda.is_available():
        model_state_dict, args = torch.load(path)
    else:
        model_state_dict, args = torch.load(path, map_location=torch.device("cpu"))
    model = Model(params=arg_to_params(args))
    model.load_state_dict_into_gnn(model_state_dict)
    print("Model loaded!")
    if print_args:
        print_arguments(args)
    model.set_eval()
    return model, args


def load_and_setup_gnn_model(path, domain_file, problem_file):
    model, args = load_model(path)
    device = "cuda" if torch.cuda.is_available() else "cpu"
    model = model.to(device)
    model.batch_search(True)
    model.update_representation(
        domain_pddl=domain_file, problem_pddl=problem_file, args=args, device=device
    )
    model.set_zero_grad()
    model.eval()
    return model
