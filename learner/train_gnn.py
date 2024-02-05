""" Main training pipeline script. """

import time
import torch
import argparse
from models.save_load import (
    arg_to_params,
    print_arguments,
    save_gnn_model_from_dict,
)
from models.gnn.loss import MSELoss
from models.gnn.model import Model
from models.gnn.train_eval import train, evaluate
from dataset.gnn import get_loaders_from_args_gnn


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument("domain_pddl")
    parser.add_argument("tasks_dir")
    parser.add_argument("plans_dir")

    # model params
    parser.add_argument("-L", "--nlayers", type=int, default=4)
    parser.add_argument("-H", "--nhid", type=int, default=64)
    parser.add_argument(
        "--aggr",
        type=str,
        default="mean",
        choices=["sum", "mean", "max"],
        help="mpnn aggregation function",
    )
    parser.add_argument(
        "--pool",
        type=str,
        default="sum",
        choices=["sum", "mean", "max"],
        help="pooling function for readout",
    )

    # optimisation params
    parser.add_argument("--lr", type=float, default=0.001)
    parser.add_argument("--patience", type=int, default=10)
    parser.add_argument("--reduction", type=float, default=0.1)
    parser.add_argument("--batch-size", type=int, default=16)
    parser.add_argument("--epochs", type=int, default=500)

    # data arguments
    parser.add_argument(
        "-r",
        "--rep",
        type=str,
        default="ilg",
        choices=["slg", "flg", "llg", "ilg"],
        help="graph representation of planning tasks",
    )
    parser.add_argument(
        "-p",
        "--planner",
        type=str,
        default="fd",
        choices=["fd", "pwl"],
        help="for converting plans to states",
    )
    parser.add_argument(
        "--small-train",
        action="store_true",
        help="Small training set: useful for debugging.",
    )

    # save file
    parser.add_argument(
        "--save-file", dest="save_file", type=str, default=None
    )

    # gpu device (if exists)
    parser.add_argument("--device", type=int, default=0)

    args = parser.parse_args()

    return args


if __name__ == "__main__":
    args = parse_args()
    print_arguments(args)

    # cuda
    device = torch.device(
        f"cuda:{args.device}" if torch.cuda.is_available() else "cpu"
    )

    # init model
    train_loader, val_loader = get_loaders_from_args_gnn(args)
    args.n_edge_labels = len(train_loader.dataset[0].edge_index)
    args.in_feat = train_loader.dataset[0].x.shape[1]
    model_params = arg_to_params(args)
    model = Model(params=model_params).to(device)
    print(f"model size (#params): {model.get_num_parameters()}")

    # argument variables
    lr = args.lr
    reduction = args.reduction
    patience = args.patience
    epochs = args.epochs

    # init optimiser
    criterion = MSELoss()
    optimiser = torch.optim.Adam(model.parameters(), lr=lr)
    scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(
        optimiser,
        mode="min",
        verbose=True,
        factor=reduction,
        patience=patience,
    )

    # train val pipeline
    print("device:", device)
    print("Training...")
    try:
        best_dict = None
        best_metric = float("inf")
        best_epoch = 0
        for e in range(epochs):
            t = time.time()

            train_stats = train(
                model, device, train_loader, criterion, optimiser
            )
            train_loss = train_stats["loss"]
            val_stats = evaluate(model, device, val_loader, criterion)
            val_loss = val_stats["loss"]
            scheduler.step(val_loss)

            # take model weights corresponding to best combined metric
            combined_metric = (train_loss + 2 * val_loss) / 3
            if combined_metric < best_metric:
                best_metric = combined_metric
                best_dict = model.model.state_dict()
                best_epoch = e

            desc = (
                f"epoch {e}, "
                f"time {time.time() - t:.1f}, "
                f"train_loss {train_loss:.2f}, "
                f"val_loss {val_loss:.2f} "
            )
            print(desc)

            lr = optimiser.param_groups[0]["lr"]
            if lr < 1e-5:
                print(f"Early stopping due to small lr: {lr}")
                break
    except KeyboardInterrupt:
        print("Early stopping due to keyboard interrupt!")

    # save model parameters
    if best_dict is not None:
        print(f"best_avg_loss {best_metric:.8f} at epoch {best_epoch}")
        args.best_metric = best_metric
        save_gnn_model_from_dict(best_dict, args)
