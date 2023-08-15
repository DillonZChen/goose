import torch
import sys
from torch.profiler import profile, record_function, ProfilerActivity
from util import eval_f1_score, eval_admissibility, eval_interval, eval_accuracy

""" Train and evaluation methods in training pipeline. """


def train(model, device, train_loader, criterion, optimiser, fast_train):
  model.train()
  train_loss = 0
  task = 'h'

  if not fast_train:
    y_true = torch.tensor([])
    y_pred = torch.tensor([])

  for data in train_loader:
    data = data.to(device)
    if task == "a":
      applicable_action = data.applicable_action.float().to(device)
    y = data.y.float().to(device)
    optimiser.zero_grad(set_to_none=True)
    # print(data.x.nelement() + 2*sum(e.shape[1] for e in data.edge_index) + data.batch.nelement() + y.nelement())
    out = model.forward(data)

    if task == "h":
      loss = criterion.forward(out, y)
    else:  # task == "a"
      # https://stackoverflow.com/a/61581099/13531424
      loss = (criterion.forward(out, y) * applicable_action).sum()
      non_zero_elements = applicable_action.sum()
      loss = loss / non_zero_elements
      # print(len(applicable_action.nonzero()))
      # print(out[applicable_action.nonzero()])
    loss.backward()
    optimiser.step()
    train_loss += loss.detach().cpu().item()

    if not fast_train:
      y_pred = torch.cat((y_pred, out.detach().cpu()))
      y_true = torch.cat((y_true, y.detach().cpu()))
  
  stats = {
    "loss": train_loss / len(train_loader),
  }
  if not fast_train:
    macro_f1, micro_f1 = eval_f1_score(y_pred=y_pred, y_true=y_true)
    stats["f1"] = macro_f1
    stats["admis"] = eval_admissibility(y_pred=y_pred, y_true=y_true)
    stats["interval"] = eval_interval(y_pred=y_pred, y_true=y_true)
    stats["acc"] = eval_accuracy(y_pred=y_pred, y_true=y_true)
  return stats


@torch.no_grad()
def evaluate(model, device, val_loader, criterion, fast_train, return_true_preds=False):
  if val_loader is None:
    return 0
  task = 'h'
  
  model.eval()
  val_loss = 0

  if not fast_train:
    y_true = torch.tensor([])
    y_pred = torch.tensor([])

  for data in val_loader:
    data = data.to(device)
    if task == "a":
      applicable_action = data.applicable_action.float().to(device)
    y = data.y.float().to(device)
    out = model.forward(data)
    if task == "h":
      loss = criterion.forward(out, y)
    else:  # task == "a"
      loss = (criterion.forward(out, y) * applicable_action).sum()
      non_zero_elements = applicable_action.sum()
      loss = loss / non_zero_elements
    val_loss += loss.detach().cpu().item()

    if not fast_train:
      y_pred = torch.cat((y_pred, out.detach().cpu()))
      y_true = torch.cat((y_true, y.detach().cpu()))

  stats = {
    "loss": val_loss / len(val_loader),
  }
  if not fast_train:
    macro_f1, micro_f1 = eval_f1_score(y_pred=y_pred, y_true=y_true)
    stats["f1"] = macro_f1
    stats["admis"] = eval_admissibility(y_pred=y_pred, y_true=y_true)
    stats["interval"] = eval_interval(y_pred=y_pred, y_true=y_true)
    stats["acc"] = eval_accuracy(y_pred=y_pred, y_true=y_true)
  if return_true_preds:
    assert not fast_train
    stats["y_true"] = y_true
    stats["y_pred"] = y_pred
  
  return stats
