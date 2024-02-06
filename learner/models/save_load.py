""" Module for dealing with model saving and loading. """
import os
import pickle
import sys
import traceback


def arg_to_gnn_params(args, out_feat=1):
    # this is an artifact of legacy code, could be just put into object input
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


def print_arguments(args):
    print("Parsed arguments:")
    for k, v in vars(args).items():
        print("{0:25}  {1}".format(k, v))
    print("___")


def save_gnn_model_from_dict(model_dict, args):
    import torch

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


def save_gnn_model(model, args):
    save_gnn_model_from_dict(model.model.state_dict(), args)
    return


def save_ml_model(model, args):
    if not hasattr(args, "save_file") or args.save_file is None:
        return
    print("Saving model...")
    save_file = args.save_file
    base_dir = os.path.dirname(save_file)
    if len(base_dir) > 0:
        os.makedirs(base_dir, exist_ok=True)
    model.setup_for_saving(save_file)
    with open(save_file, "wb") as handle:
        pickle.dump(model, handle)
    print("Model saved!")
    print("Model parameter file:", save_file)
    return


def load_gnn_model(path, print_args=False):
    # returns (GNN, Args)
    import torch
    from models.gnn.core import Model

    print(f"Loading model from {path}...")
    if not os.path.exists(path):
        print(f"Model not found at {path}")
        exit(-1)
    if torch.cuda.is_available():
        model_state_dict, args = torch.load(path)
    else:
        model_state_dict, args = torch.load(
            path, map_location=torch.device("cpu")
        )
    model = Model(params=arg_to_gnn_params(args))
    model.load_state_dict_into_gnn(model_state_dict)
    print("Model loaded!")
    if print_args:
        print_arguments(args)
    model.set_eval()
    return model, args


def load_ml_model(path):
    path = os.path.abspath(path)
    sys.path.append('learner/')
    with open(path, "rb") as handle:
        try:
            model = pickle.load(handle)
        except Exception:
            print(traceback.format_exc(), flush=True)
            exit(-1)
    return model


def load_gnn_model_and_setup(path, domain_file, problem_file):
    import torch

    model, args = load_gnn_model(path)
    device = "cuda" if torch.cuda.is_available() else "cpu"
    model = model.to(device)
    model.batch_search(True)
    model.update_representation(
        domain_pddl=domain_file,
        problem_pddl=problem_file,
        args=args,
        device=device,
    )
    model.set_zero_grad()
    model.eval()
    return model


def load_ml_model_and_setup(path, domain_file, problem_file):
    try:
        print(f"Entered Python code for loading model", flush=True)
        model = load_ml_model(path)
        model.setup_after_loading(path)
        print("Updating representation", flush=True)
        model.update_representation(
            domain_pddl=domain_file, problem_pddl=problem_file
        )
        print("Representation updated!", flush=True)
        model.eval()
        print("Set to eval mode.", flush=True)
    except Exception:
        print(traceback.format_exc(), flush=True)
    return model
