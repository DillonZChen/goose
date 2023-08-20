""" Main training pipeline script. """

import time
import torch
import argparse
import gnns
import representation
from gnns import *
from tqdm.auto import tqdm, trange
from util.stats import *
from util.save_load import *
from util import train, evaluate
from dataset.dataset import get_loaders_from_args_gnn


def create_parser():
  parser = argparse.ArgumentParser()
  parser.add_argument('--device', type=int, default=0)
  parser.add_argument('-d', '--domain', default="goose-di")
  parser.add_argument('-t', '--task', default='h', choices=["h", "a"], 
                      help="predict value or action (currently only h is supported)")

  # model params
  parser.add_argument('-m', '--model', type=str, required=True, choices=gnns.GNNS)
  parser.add_argument('-L', '--nlayers', type=int, default=16)
  parser.add_argument('-H', '--nhid', type=int, default=64)
  parser.add_argument('--share-layers', action='store_true')
  parser.add_argument('--aggr', type=str, default="mean")
  parser.add_argument('--pool', type=str, default="sum")
  parser.add_argument('--drop', type=float, default=0.0, 
                      help="probability of an element to be zeroed")
  parser.add_argument('--vn', action='store_true',
                      help="use virtual nodes (doubles runtime)")

  # optimisation params
  parser.add_argument('--loss', type=str, choices=["mse", "wmse", "pemse"], default="mse")
  parser.add_argument('--lr', type=float, default=0.001)
  parser.add_argument('--patience', type=int, default=10)
  parser.add_argument('--reduction', type=float, default=0.1)
  parser.add_argument('--batch-size', type=int, default=16)
  parser.add_argument('--epochs', type=int, default=2000)

  # data arguments
  parser.add_argument('-r', '--rep', type=str, required=True, choices=representation.REPRESENTATIONS)
  parser.add_argument('-n', '--max-nodes', type=int, default=-1, 
                      help="max nodes for generating graphs (-1 means no bound)")
  parser.add_argument('-c', '--cutoff', type=int, default=-1, 
                      help="max cost to learn (-1 means no bound)")
  parser.add_argument('--small-train', action="store_true", 
                      help="Small train set: useful for debugging.")

  # save file
  parser.add_argument('--save-file', dest="save_file", type=str, default=None)

  # anti verbose
  parser.add_argument('--no-tqdm', dest='tqdm', action='store_false')
  parser.add_argument('--fast-train', action='store_true', 
                      help="ignore some additional computation of stats, does not change the training algorithm")
  
  return parser

def check_config(args):
  pass  # TODO check model is compatible with representation

if __name__ == "__main__":
  parser = create_parser()
  args = parser.parse_args()
  check_config(args)
  print_arguments(args)

  # cuda
  device = torch.device(f'cuda:{args.device}' if torch.cuda.is_available() else 'cpu')

  # init model
  train_loader, val_loader = get_loaders_from_args_gnn(args)
  args.n_edge_labels = representation.REPRESENTATIONS[args.rep].n_edge_labels
  args.in_feat = train_loader.dataset[0].x.shape[1]
  model_params = arg_to_params(args)
  model = GNNS[args.model](params=model_params).to(device)
  print(f"model size (#params): {model.get_num_parameters()}")

  # argument variables
  lr = args.lr
  reduction = args.reduction
  patience = args.patience
  epochs = args.epochs
  loss_fn = args.loss
  fast_train = args.fast_train

  # init optimiser
  criterion = LOSS[loss_fn]()
  optimiser = torch.optim.Adam(model.parameters(), lr=lr)
  scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(
    optimiser,
    mode='min',
    verbose=True,
    factor=reduction,
    patience=patience
  )

  # train val pipeline
  print("Training...")
  try:
    if args.tqdm:
      pbar = trange(epochs)
    else:
      pbar = range(epochs)
    best_dict = None
    best_metric = float('inf')
    best_epoch = 0
    for e in pbar:
      t = time.time()

      train_stats = train(model, device, train_loader, criterion, optimiser, fast_train=fast_train)
      train_loss = train_stats['loss']
      val_stats = evaluate(model, device, val_loader, criterion, fast_train=fast_train)
      val_loss = val_stats['loss']
      scheduler.step(val_loss)

      # take model weights corresponding to best combined metric
      combined_metric = (train_loss + 2*val_loss)/3
      if combined_metric < best_metric:
        best_metric = combined_metric
        best_dict = model.model.state_dict()
        best_epoch = e

      if fast_train:  # does not compute metrics like f1 score
        desc = f"epoch {e}, " \
               f"train_loss {train_loss:.2f}, " \
               f"val_loss {val_loss:.2f}, " \
               f"time {time.time() - t:.1f}"
      else:  # computes all metrics
        desc = f"epoch {e}, " \
               f"train_f1 {train_stats['f1']:.1f}, " \
               f"val_f1 {val_stats['f1']:.1f}, " \
               f"train_int {train_stats['interval']}, " \
               f"val_int {val_stats['interval']}, " \
               f"train_adm {train_stats['admis']:.1f}, " \
               f"val_adm {val_stats['admis']:.1f}, " \
               f"train_loss {train_loss:.2f}, " \
               f"val_loss {val_loss:.2f}, " \
               f"time {time.time() - t:.1f}"
        
      lr = optimiser.param_groups[0]['lr']
      if args.tqdm:
        tqdm.write(desc)
        pbar.set_description(desc)
      else:
        print(desc)

      if lr < 1e-5:
        print(f"Early stopping due to small lr: {lr}")
        break
  except KeyboardInterrupt:
    print("Early stopping due to keyboard interrupt!")

  # save model parameters
  if best_dict is not None:
    print(f"best_avg_loss {best_metric:.8f} at epoch {best_epoch}")
    args.best_metric = best_metric
    save_model_from_dict(best_dict, args)
