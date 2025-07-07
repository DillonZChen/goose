import argparse
import logging
import time

import torch
from torch.nn import BCELoss, BCEWithLogitsLoss, Module, MSELoss
from torch.optim import Optimizer
from torch.utils.data import DataLoader

from learning.predictor.neural_network.weights_dict import WeightsDict


def optimise_weights(
    model: Module,
    device: torch.device,
    train_loader: DataLoader,
    val_loader: DataLoader,
    opts: argparse.Namespace,
) -> WeightsDict:

    if opts.policy_type.is_policy_function():
        logging.info("Optimising with BCELoss")
        criterion = BCEWithLogitsLoss()
    else:
        logging.info("Optimising with MSELoss")
        criterion = MSELoss()
    optimiser = torch.optim.Adam(model.parameters(), lr=opts.learning_rate)
    scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(
        optimiser,
        mode="min",
        factor=opts.reduction,
        patience=opts.patience,
    )

    model_dict = None
    best_metric = float("inf")
    best_epoch = None
    train_loss = None
    val_loss = None

    try:
        for epoch in range(opts.epochs):
            t = time.time()

            train_loss_loc = train_step(model, device, train_loader, criterion, optimiser)
            val_loss_loc = val_step(model, device, val_loader, criterion)
            scheduler.step(val_loss_loc)

            # take gnn weights corresponding to best combined metric
            combined_metric = (train_loss_loc + 2 * val_loss_loc) / 3
            if combined_metric < best_metric:
                best_metric = combined_metric
                model_dict = model.state_dict()
                train_loss = train_loss_loc
                val_loss = val_loss_loc

            lr = optimiser.param_groups[0]["lr"]
            t = time.time() - t

            print(
                ", ".join([f"{epoch=}", f"{t=:.8f}", f"{train_loss_loc=:.8f}", f"{val_loss_loc=:.8f}", f"{lr=:.1e}"])
            )

            if lr < 1e-5:
                logging.info(f"Early stopping due to small {lr=:.1e}")
                break
        logging.info(f"Stopping reaching bound of {opts.epochs} epochs")
    except KeyboardInterrupt:
        logging.info(f"Early stopping due to keyboard interrupt!")

    logging.info(f"{best_epoch=}")
    logging.info(f"{train_loss=:.8f}")
    logging.info(f"{val_loss=:.8f}")

    return model_dict


def train_step(
    model: Module,
    device: torch.device,
    train_loader: DataLoader,
    criterion: Module,
    optimiser: Optimizer,
) -> float:
    model.train()
    train_loss = 0

    for data in train_loader:
        data = data.to(device)
        y_true = data.y.float().to(device)
        optimiser.zero_grad(set_to_none=True)
        y_pred = model.forward(x=data.x, edge_indices_list=data.edge_index, batch=data.batch)

        loss = criterion.forward(y_pred, y_true)
        loss.backward()
        optimiser.step()
        train_loss += loss.detach().cpu().item()

    train_loss /= len(train_loader)
    return train_loss


@torch.no_grad()
def val_step(
    model: Module,
    device: torch.device,
    val_loader: DataLoader,
    criterion: Module,
) -> float:
    model.eval()
    val_loss = 0

    for data in val_loader:
        data = data.to(device)
        h_true = data.y.float().to(device)
        h_pred = model.forward(x=data.x, edge_indices_list=data.edge_index, batch=data.batch)

        loss = criterion.forward(h_pred, h_true)
        val_loss += loss.detach().cpu().item()

    val_loss /= len(val_loader)
    return val_loss
