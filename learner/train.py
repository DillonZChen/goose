import os
import time
import torch
import argparse
import torch_geometric
import random
import numpy as np
import configuration

from models import *
from tqdm.auto import tqdm, trange
from util.stats import *
from util.save_load import *
from loss import LOSS
from util import train, evaluate
from util.dataset import get_loaders_from_args


def main():
    parser = configuration.create_parser()
    args = parser.parse_args()
    configuration.check_config(args)
    print_arguments(args)

    # cuda
    device = torch.device(f'cuda:{args.device}' if torch.cuda.is_available() else 'cpu')

    # init model
    if args.pretrained is not None:
      domain = args.domain
      pretrained = args.pretrained
      model, args = load_model(args.pretrained, print_args=True)
      args.domain = domain
      args.pretrained = pretrained
      model = model.to(device)
      train_loader, val_loader, test_loader = get_loaders_from_args(args)
    else:
      train_loader, val_loader, test_loader = get_loaders_from_args(args)
      args.n_edge_labels = representation.N_EDGE_TYPES[args.rep]
      args.in_feat = train_loader.dataset[0].x.shape[1]
      model_params = arg_to_params(args)
      model = GNNS[args.model](params=model_params).to(device)

    lr = args.lr
    reduction = args.reduction
    patience = args.patience
    epochs = args.epochs
    task = args.task
    loss_fn = args.loss
    val = args.val
    fast_train = args.fast_train

    # init optimiser
    criterion = LOSS[task][loss_fn]()
    optimiser = torch.optim.Adam(model.parameters(), lr=lr, weight_decay=args.decay)
    scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(optimiser,
                                                           mode='min',
                                                           verbose=True,
                                                           factor=reduction,
                                                           patience=patience)

    print(f"model size (#params): {model.get_num_parameters()}")

    # train val pipeline
    print("Training...")
    try:
      pbar = trange(epochs)
      best_dict = None
      best_metric = float('inf')
      best_epoch = 0
      for e in pbar:
        t = time.time()
        train_stats = train(model, device, train_loader, criterion, optimiser, task=task, fast_train=fast_train)
        train_loss = train_stats['loss']
        if val:
          val_stats = evaluate(model, device, val_loader, criterion, task=task, fast_train=fast_train)
          val_loss = val_stats['loss']
          scheduler.step(val_loss)
          combined_metric = (train_loss + 2*val_loss)/3
          if combined_metric < best_metric:
            best_metric = combined_metric
            best_dict = model.model.state_dict()
            best_epoch = e
          if fast_train:
            desc = f"epoch {e}, " \
                  f"train_loss {train_loss:.2f}, " \
                  f"val_loss {val_loss:.2f}, " \
                  f"time {time.time() - t:.1f}"
          else:
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
        else:  # no validation set
          val_interval = float('inf')
          val_loss = float('inf')
          scheduler.step(train_loss)
          desc = f"epoch {e}, " \
                f"train_f1 {train_stats['f1']:.1f}, " \
                f"train_int {train_stats['interval']}, " \
                f"train_adm {train_stats['admis']:.1f}, " \
                f"train_loss {train_loss:.2f}, " \
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
        # elif np.abs(train_macro_f1 - 100) < 1e-3 and np.abs(val_macro_f1 - 100) < 1e-3:
        #     print(f"Early stopping due to almost perfect f1")
        #     break
        # elif train_interval == 0 and train_loss < 0.1 and val_interval == 0 and val_loss < 0.1:
        #     print(f"Early stopping due to perfect interval and low loss")
        #     break
    except KeyboardInterrupt:
      print("Early stopping due to keyboard interrupt!")

    # save model parameters
    if best_dict is not None:
      print(f"best_avg_loss {best_metric:.8f} at epoch {best_epoch}")
      args.best_metric = best_metric
      save_model_from_dict(best_dict, args)
    else:
      save_model(model, args)

    if len(test_loader.dataset) > 0:
      stats = evaluate(model, device, test_loader, criterion, task=task, fast_train=fast_train)
      print(f"Test results:")
      desc = f"test_f1 {stats['f1']:.1f}, " \
             f"test_adm {stats['admis']:.1f}, " \
             f"test_loss {stats['loss']:.2f}, " \
             f"test_interval {stats['interval']} "
      print(desc)
    return


if __name__ == "__main__":
    main()
