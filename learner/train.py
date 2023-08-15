import time
import torch
import configuration

from models import *
from tqdm.auto import tqdm, trange
from util.stats import *
from util.save_load import *
from util import train, evaluate
from dataset.dataset import get_loaders_from_args

""" Main training pipeline script. """


if __name__ == "__main__":
  parser = configuration.create_parser()
  args = parser.parse_args()
  configuration.check_config(args)
  print_arguments(args)

  # cuda
  device = torch.device(f'cuda:{args.device}' if torch.cuda.is_available() else 'cpu')

  # init model
  train_loader, val_loader = get_loaders_from_args(args)
  args.n_edge_labels = representation.N_EDGE_TYPES[args.rep]
  args.in_feat = train_loader.dataset[0].x.shape[1]
  model_params = arg_to_params(args)
  model = GNNS[args.model](params=model_params).to(device)

  lr = args.lr
  reduction = args.reduction
  patience = args.patience
  epochs = args.epochs
  loss_fn = args.loss
  fast_train = args.fast_train

  # init optimiser
  criterion = LOSS[loss_fn]()
  optimiser = torch.optim.Adam(model.parameters(), lr=lr)
  scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(optimiser,
                                                          mode='min',
                                                          verbose=True,
                                                          factor=reduction,
                                                          patience=patience)

  print(f"model size (#params): {model.get_num_parameters()}")

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
  else:
    save_model(model, args)
