import numpy as np
import torch
from typing import Tuple
from torch import Tensor
from sklearn.metrics import f1_score
from sklearn.metrics import accuracy_score

""" Module containing metrics for inference only. """


def f1_macro(y_true: np.array, y_pred: np.array) -> float:
  y_true = np.rint(y_true).astype(int)
  y_pred = np.rint(y_pred).astype(int)
  return f1_score(y_true, y_pred, average='macro')

@torch.no_grad()
def eval_accuracy(y_pred: Tensor, y_true: Tensor):
  try:
    y_pred = torch.round(y_pred).long()
  except:
    pass
  y_true = y_true.long()
  acc = accuracy_score(y_true, y_pred) * 100
  return acc

@torch.no_grad()
def eval_f1_score(y_pred: Tensor, y_true: Tensor) -> Tuple[int, int]:
  try:
    y_pred = torch.round(y_pred).long()
  except:
    pass
  y_true = y_true.long()
  f1_macro = f1_score(y_true, y_pred, average='macro') * 100
  f1_micro = f1_score(y_true, y_pred, average='micro') * 100
  return f1_macro, f1_micro

@torch.no_grad()
def eval_admissibility(y_pred: Tensor, y_true: Tensor) -> int:
  y_true = y_true.int()
  y_true_min = 0
  y_true_max = round(torch.max(y_true).item())
  admissibility_cnt = torch.zeros((y_true_max-y_true_min+1))
  total_cnt = torch.zeros((y_true_max-y_true_min+1))
  admissibility = torch.round(y_pred) <= y_true
  for i in range(len(y_true)):
    admissibility_cnt[y_true[i]] += admissibility[i]
    total_cnt[y_true[i]] += 1
  admissibility_cnt /= total_cnt
  admissibility_cnt[total_cnt==0] = 0
  admis = torch.sum(admissibility_cnt).item() / torch.sum(total_cnt > 0).item() * 100
  return admis

@torch.no_grad()
def eval_interval(y_pred: Tensor, y_true: Tensor) -> int:
  max_true = torch.max(y_true).item()
  min_true = torch.min(y_true).item()
  max_pred = torch.max(y_pred).item()
  min_pred = torch.min(y_pred).item()
  worst = round(max(np.abs(max_true-min_pred), np.abs(min_true-max_pred)))
  difference = [0 for _ in range(worst+1)]
  for i in range(len(y_true)):
    diff = round(np.abs(y_true[i].item() - y_pred[i].item()))
    difference[diff] += 1
  bound = round(0.95 * len(y_true))
  cum = 0
  for i in range(len(difference)):
    cum += difference[i]
    if cum >= bound:
      return i
  assert 0  # should never get here
    
