""" Module for dealing with model saving and loading. """
import os
import joblib


_TRAINED_GNNS_SAVE_DIR = "trained_models_gnn"
_TRAINED_KERNELS_SAVE_DIR = "trained_models_kernel"
   

def arg_to_params(args, in_feat=4, out_feat=1):
  model_name = args.model
  nlayers = args.nlayers
  nhid = args.nhid
  in_feat = args.in_feat
  n_edge_labels = args.n_edge_labels
  share_layers = args.share_layers
  task = args.task
  pool = args.pool
  aggr = args.aggr
  vn = args.vn
  rep = args.rep
  model_params = {
    'model_name': model_name,
    'in_feat': in_feat,
    'out_feat': out_feat,
    'nlayers': nlayers,
    'share_layers': share_layers,
    'n_edge_labels': n_edge_labels,
    'nhid': nhid,
    'aggr': aggr,
    'pool': pool,
    'task': task,
    'rep': rep,
    'vn': vn,
  }
  return model_params


def print_arguments(args, ignore_params=set()):
  if hasattr(args, 'pretrained') and args.pretrained is not None:
      return
  print("Parsed arguments:")
  for k, v in vars(args).items():
      if k in ignore_params.union({"device", "optimal", "save_model", "save_file", "no_tqdm", "tqdm", "fast_train"}):
          continue
      print('{0:20}  {1}'.format(k, v))


def save_gnn_model_from_dict(model_dict, args):
  import torch
  if not hasattr(args, "save_file") or args.save_file is None:
    return
  print("Saving model...")
  model_file_name = args.save_file.replace(".dt", "")
  os.makedirs(_TRAINED_GNNS_SAVE_DIR, exist_ok=True)
  path = f'{_TRAINED_GNNS_SAVE_DIR}/{model_file_name}.dt'
  torch.save((model_dict, args), path)
  print("Model saved!")
  print("Model parameter file:")
  print(model_file_name)
  return


def save_gnn_model(model, args):
  save_gnn_model_from_dict(model.model.state_dict(), args)
  return


def save_kernel_model(model, args):
  if not hasattr(args, "save_file") or args.save_file is None:
    return
  print("Saving model...")
  model_file_name = args.save_file.replace(".joblib", "")
  os.makedirs(_TRAINED_KERNELS_SAVE_DIR, exist_ok=True)
  path = f'{_TRAINED_KERNELS_SAVE_DIR}/{model_file_name}.joblib'
  joblib.dump((model, args), path)
  print("Model saved!")
  print("Model parameter file:")
  print(model_file_name)
  return


def load_gnn_model(path, print_args=False, jit=False, ignore_subdir=False):
  # returns (GNN, Args)
  import torch
  from gnns import GNNS
  print("Loading model...")
  assert ".pt" not in path, f"Found .pt in path {path}"
  if ".dt" not in path:
      path = path+".dt"
  if not ignore_subdir and _TRAINED_GNNS_SAVE_DIR not in path:
      path = _TRAINED_GNNS_SAVE_DIR + "/" + path
  try:
    if torch.cuda.is_available():
        model_state_dict, args = torch.load(path)
    else:
        model_state_dict, args = torch.load(path, map_location=torch.device('cpu'))
  except:
    print(f"Model not found at {path}")
    exit(-1)
  # update legacy naming
  if "dg-el" in args.rep:
      args.rep = args.rep.replace("dg-el", "lg")
  model = GNNS[args.model](params=arg_to_params(args), jit=jit)
  model.load_state_dict_into_gnn(model_state_dict)
  print("Model loaded!")
  if print_args:
      print_arguments(args)
  model.eval()
  return model, args


def load_kernel_model(path, ignore_subdir=False):
  if ".joblib" not in path:
      path = path+".joblib"
  if not ignore_subdir and _TRAINED_KERNELS_SAVE_DIR not in path:
      path = _TRAINED_KERNELS_SAVE_DIR + "/" + path
  model, args = joblib.load(path)
  return model, args


def load_gnn_model_and_setup(path, domain_file, problem_file):
  import torch
  model, args = load_gnn_model(path, ignore_subdir=True)
  device = "cuda" if torch.cuda.is_available() else "cpu"
  model = model.to(device)
  model.batch_search(True)
  model.update_representation(domain_pddl=domain_file,
                              problem_pddl=problem_file,
                              args=args,
                              device=device)
  model.set_zero_grad()
  model.eval()
  return model


def load_kernel_model_and_setup(path, domain_file, problem_file):
  model, args = load_kernel_model(path, ignore_subdir=True)
  model.update_representation(domain_pddl=domain_file, problem_pddl=problem_file)
  # model.write_representation_to_file()
  model.eval()
  return model
  
