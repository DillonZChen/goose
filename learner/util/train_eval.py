import torch
import sys
from torch.profiler import profile, record_function, ProfilerActivity
from util import eval_f1_score, eval_admissibility, eval_interval, eval_accuracy

def train_adapt(model, device, src_loader, tar_loader, criterion, optimiser):
  model.train()
  train_loss = 0
  train_mse = 0
  train_penalty = 0
  batches = min(len(src_loader), len(tar_loader))

  for _ in range(batches):
    src_data = next(iter(src_loader)).to(device)
    tar_data = next(iter(tar_loader)).to(device)

    src_emb = model.embeddings(src_data)
    tar_emb = model.embeddings(tar_data)
    
    pred = model.forward_from_embeddings(src_emb)
    y = src_data.y.float().to(device)

    optimiser.zero_grad(set_to_none=True)
    loss, mse, penalty = criterion.forward(pred, y, src_emb, tar_emb)

    loss.backward()
    optimiser.step()
    train_loss += loss.detach().cpu().item()
    train_mse += mse.detach().cpu().item()
    train_penalty += penalty.detach().cpu().item()
  
  train_loss /= batches
  train_mse /= batches
  train_penalty /= batches
  
  return train_loss, train_mse, train_penalty


@torch.no_grad()
def evaluate_adapt(model, device, src_loader, tar_loader, criterion):
  model.eval()
  val_loss = 0
  val_mse = 0
  val_penalty = 0
  batches = min(len(src_loader), len(tar_loader))

  for _ in range(batches):
    src_data = next(iter(src_loader)).to(device)
    tar_data = next(iter(tar_loader)).to(device)

    src_emb = model.embeddings(src_data)
    tar_emb = model.embeddings(tar_data)
    
    pred = model.forward_from_embeddings(src_emb)
    y = src_data.y.float().to(device)

    loss, mse, penalty = criterion.forward(pred, y, src_emb, tar_emb)

    val_loss += loss.detach().cpu().item()
    val_mse += mse.detach().cpu().item()
    val_penalty += penalty.detach().cpu().item()
  
  val_loss /= batches
  val_mse /= batches
  val_penalty /= batches
  
  return val_loss, val_mse, val_penalty



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
