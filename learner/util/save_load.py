import os
import torch
import datetime
import representation
from models import *


def arg_to_params(args, in_feat=4, out_feat=1):
    model_name = args.model
    nlayers = args.nlayers
    nhid = args.nhid
    nhid2 = args.nhid2 if hasattr(args, "nhid2") else None
    nheads = args.nheads if hasattr(args, "nheads") else 0
    drop = args.drop if hasattr(args, "drop") else 0
    in_feat = args.in_feat if hasattr(args, "in_feat") else in_feat
    n_edge_labels = args.n_edge_labels if hasattr(args, "n_edge_labels") else 0
    share_layers = args.share_layers
    task = args.task
    pool = args.pool if hasattr(args, "pool") else "max"
    double = args.double
    vn = args.vn if hasattr(args, "vn") else False
    rep = args.rep
    model_params = {
        'model_name': model_name,
        'in_feat': in_feat,
        'out_feat': out_feat,
        'nlayers': nlayers,
        'share_layers': share_layers,
        'n_edge_labels': n_edge_labels,
        'nhid': nhid,
        'nhid2': nhid2,
        'drop': drop,
        'pool': pool,
        'nheads': nheads,
        'double': double,
        'task': task,
        'rep': rep,
        'vn': vn,
    }
    return model_params


def print_arguments(args, ignore_params=set()):
    if hasattr(args, 'pretrained') and args.pretrained is not None:
        return
    print("Parsed arguments:")
    if args.task == "a":
        args.loss = "bce"
    for k, v in vars(args).items():
        if args.model != "FFNet" and k == "nheads":
            continue
        if k in {"d_test", "p_test", "device", "optimal", "z", "save_model", "parser", "directed", "edge_labels", "unseen", "save_file", "no_tqdm", "tqdm", "fast_train"}:
            continue
        if k in ignore_params:
            continue
        print('{0:20}  {1}'.format(k, v))


def save_model_from_dict(model_dict, args):
    if not hasattr(args, "save_file") and args.save_file is not None:
      return
    print("Saving model...")
    save_dir = 'trained_models'
    os.makedirs(f"{save_dir}/", exist_ok=True)
    model_file_name = args.save_file.replace(".dt", "")
    path = f'{save_dir}/{model_file_name}.dt'
    torch.save((model_dict, args), path)
    print("Model saved!")
    print("Model parameter file:")
    print(model_file_name)
    return


def save_model(model, args, prefix="", save=True):
    save_model_from_dict(model.model.state_dict(), args, prefix, save)
    return


def load_model(path, print_args=False, jit=True, ignore_subdir=False):
    print("Loading model...")
    assert ".pt" not in path
    if ".dt" not in path:
        path = path+".dt"
    if not ignore_subdir and "trained_models" not in path:
        path = "trained_models/" + path
    if torch.cuda.is_available():
        model_state_dict, args = torch.load(path)
    else:
        model_state_dict, args = torch.load(path, map_location=torch.device('cpu'))
    model = GNNS[args.model](params=arg_to_params(args), jit=jit)
    model.load_state_dict_into_gnn(model_state_dict)
    print("Model loaded!")
    if print_args:
        print_arguments(args)
    model.eval()
    return model, args


def load_model_and_setup(path, domain_file, problem_file):
    model, args = load_model(path, ignore_subdir=True)
    device = "cuda" if torch.cuda.is_available() else "cpu"
    model = model.to(device)
    model.batch_search(True)
    model.update_representation(domain_pddl=domain_file,
                                problem_pddl=problem_file,
                                args=args,
                                device=device)
    # model.add_node_features(args=model_args)
    model.set_zero_grad()
    model.eval()
    return model


def load_model_obj(path):
    if ".pt" not in path:
        path = path+".pt"
    if "trained_models" not in path:
        path = "trained_models/" + path
    model = torch.load(path)
    return model

