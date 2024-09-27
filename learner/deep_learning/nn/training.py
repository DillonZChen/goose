import random

import torch
from torch_geometric.loader import DataLoader

from learner.model import Model


def train_ranker(model, device, data_groups, criterion, optimiser, succ_batch_size):
    model.train()
    train_loss = 0
    cnt = 0
    random.shuffle(data_groups)
    for i in range(0, len(data_groups), succ_batch_size):
        data_lists = []
        good_groups = []
        maybe_bad_groups = []
        def_bad_groups = []
        for data_list, good_group, maybe_bad_group, def_bad_group in data_groups[
            i : min(i + succ_batch_size, len(data_groups) - 1)
        ]:
            good_group = [g + len(data_lists) for g in good_group]
            maybe_bad_group = [b + len(data_lists) for b in maybe_bad_group]
            def_bad_group = [d + len(data_lists) for d in def_bad_group]
            data_lists += data_list
            good_groups.append(good_group)
            maybe_bad_groups.append(maybe_bad_group)
            def_bad_groups.append(def_bad_group)
        if len(data_lists) == 0:
            print(i, len(data_groups))
            continue
        data = next(
            iter(DataLoader(data_lists, batch_size=len(data_lists), shuffle=False))
        ).to(device)
        optimiser.zero_grad(set_to_none=True)
        h = model.forward(data)
        
        # see ranking_loss.py
        loss = criterion.forward(h, good_groups, maybe_bad_groups, def_bad_groups)
        loss.backward()
        cnt += criterion.cnt
        criterion.cnt = 0
        optimiser.step()
        train_loss += loss.detach().cpu().item()
    return {"loss": train_loss}


def train(model: Model, device, loader, criterion, optimiser, return_outputs=False):
    model.train()
    train_loss = 0

    if return_outputs:
        y_trues = []
        y_preds = []

    for data in loader:
        data = data.to(device)
        h_true = data.y.float().to(device)

        optimiser.zero_grad(set_to_none=True)
        h_pred = model.forward(data)

        loss = criterion.forward(h_pred, h_true)
        loss.backward()
        optimiser.step()
        train_loss += loss.detach().cpu().item()

        if return_outputs:
            y_trues.append(h_true.detach().cpu())
            y_preds.append(h_pred.detach().cpu())

    output = {
        "loss": train_loss / len(loader),
    }
    if return_outputs:
        output["y_true"] = torch.cat(y_trues)
        output["y_pred"] = torch.cat(y_preds)

    return output


@torch.no_grad()
def evaluate(model: Model, device, loader, criterion, return_outputs=False):
    model.eval()
    val_loss = 0

    if return_outputs:
        y_trues = []
        y_preds = []

    for data in loader:
        data = data.to(device)
        y_true = data.y.float().to(device)
        y_pred = model.forward(data)

        loss = criterion.forward(y_pred, y_true)
        val_loss += loss.detach().cpu().item()

        if return_outputs:
            y_trues.append(y_true.detach().cpu())
            y_preds.append(y_pred.detach().cpu())

    output = {
        "loss": val_loss / len(loader),
    }
    if return_outputs:
        output["y_true"] = torch.cat(y_trues)
        output["y_pred"] = torch.cat(y_preds)

    return output
