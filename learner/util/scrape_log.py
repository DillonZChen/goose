import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from tqdm import trange
from util.dataset import get_loaders_from_args
from loss import LOSS
from torch_geometric.loader import DataLoader
from util.train_eval import evaluate
from util.stats import get_stats
from representation.node_features import add_features
from util.transform import preprocess_data
import torch

from gen_data.graphs import get_graph_data

def scrape_log(file, train_only):
  stats = {
    "epochs": -1,
    "train_f1": -1,
    "val_f1": -1,
    "train_int": -1,
    "val_int": -1,
    "train_adm": -1,
    "val_adm": -1,
    "train_loss": -1,
    "val_loss": -1,
    "time": 0,
    "model_path": -1,
    "best_avg_loss": float('inf'),
  }
  
  model_time = False
  arguments = False

  for line in open(file, 'r').readlines():
    line = line.replace(",", "")
    toks = line.split()
    if len(toks) == 0:
        continue
    
    if "Loading " in line:
        arguments = False
    if arguments:
        if len(toks) == 1:
          stats[toks[0]] = ""
        else:
          assert len(toks) == 2
          stats[toks[0]] = toks[1]
    if "Parsed arguments" in line:
        arguments = True

    if toks[0] == "epoch":
      stats["epochs"] = int(toks[1])
      try:
        if train_only: 
          stats["train_f1"] =   float(toks[3])
          stats["train_int"] =  int(toks[5])
          stats["train_adm"] =  float(toks[7])
          stats["train_loss"] = float(toks[9])
          stats["time"] +=      float(toks[11])
        else:
          stats["train_f1"] =   float(toks[3])
          stats["val_f1"] =     float(toks[5])
          stats["train_int"] =  int(toks[7])
          stats["val_int"] =    int(toks[9])
          stats["train_adm"] =  float(toks[11])
          stats["val_adm"] =    float(toks[13])
          stats["train_loss"] = float(toks[15])
          stats["val_loss"] =   float(toks[17])
          stats["time"] +=      float(toks[19])
      except:
          pass # fast train

    if model_time:
        stats["model_path"] = line.replace("\n", "")
        model_time = False
    if "Model parameter file" in line:
        model_time = True
    if toks[0] == "best_avg_loss":
       stats["best_avg_loss"] = float(toks[1])
       
  return stats

def scrape_gen_plot_from_model(model, args, h="opt"): #assume loaded to avoid circular import
  device = torch.device(f'cuda:{args.device}' if torch.cuda.is_available() else 'cpu')
  print()
  print(f"Getting data above cutoff...")
  dataset = get_graph_data(representation=args.rep, task=args.task, domain=args.domain)
  dataset = add_features(dataset, args)
  dataset = preprocess_data(model_name=args.model,
                            data_list=dataset,
                            heuristic=h,
                            c_lo=args.cutoff,
                            n_hi=10000,
                            c_hi=-1,
                            small_train=False)
  get_stats(dataset=dataset, task="h", desc="Unseen y dataset")
  print()
  print(f"Getting data below cutoff...")
  train_loader, _, test_loader = get_loaders_from_args(args)
  dataset += [data for data in test_loader.dataset]

  train_stats = evaluate(model, device, train_loader, LOSS[args.task][args.loss](), args.task, return_true_preds=True, fast_train=False)

  y_trues = torch.tensor([])
  y_preds = torch.tensor([])
  max_y = 0
  for data in dataset:
     max_y = round(max(max_y, data.y))  # round for lmcut rep. as float
  dataset_per_y = [[] for _ in range(max_y+1)]
  for data in dataset:
     dataset_per_y[round(data.y)].append(data)
  metrics_per_y = [{} for _ in range(max_y+1)]
  for y in trange(max_y + 1):
    if len(dataset_per_y[y]) > 0:
      loader = DataLoader(dataset_per_y[y], batch_size=args.batch_size, shuffle=False)
      # val_loss, macro_f1, micro_f1, admis, interval, acc, y_true, y_pred
      stats = evaluate(model, device, loader, LOSS[args.task][args.loss](), args.task, return_true_preds=True, fast_train=False)
      metrics_per_y[y]["loss"] = stats['loss']
      metrics_per_y[y]["acc"] = stats['acc']
      metrics_per_y[y]["admis"] = stats['admis']
      metrics_per_y[y]["interval"] = stats['interval']
      y_preds = torch.cat((y_preds, stats['y_pred']))
      y_trues = torch.cat((y_trues, stats['y_true']))
    else:
      metrics_per_y[y]["loss"] = -1
      metrics_per_y[y]["acc"] = -1
      metrics_per_y[y]["admis"] = -1
      metrics_per_y[y]["interval"] = -1
  return metrics_per_y, y_trues, y_preds, train_stats
