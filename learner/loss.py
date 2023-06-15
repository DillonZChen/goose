import torch
import torch.nn.functional as F

from torch import Tensor
from torch.nn import MSELoss, BCEWithLogitsLoss

class MMD:
  def __init__(self, weight: float) -> None:
    self.weight = weight
    self.mse = MSELoss()

  def forward(self, input: Tensor, target: Tensor, D_s: Tensor, D_t: Tensor):
    mse = self.mse.forward(input, target)

    m_s = torch.mean(D_s, dim=0)
    m_t = torch.mean(D_t, dim=0)
    mmd = torch.linalg.norm(m_s - m_t) ** 2
    return mse + self.weight * mmd, mse, mmd

class DeepCORAL:
  def __init__(self, weight: float) -> None:
    self.weight = weight
    self.mse = MSELoss()

  @staticmethod
  def compute_covariance(D):
      n = D.shape[0]

      col = torch.ones((1, n), device=D.device) @ D

      c = D.T @ D - ((col.T @ col) / n)
      c /= n - 1

      return c

  def forward(self, input: Tensor, target: Tensor, D_s: Tensor, D_t: Tensor):
    mse = self.mse.forward(input, target)

    d = D_s.shape[1]
    C_s = self.compute_covariance(D_s)
    C_t = self.compute_covariance(D_t)
    coral = torch.norm(C_s - C_t, p='fro')**2
    coral /= 4*d*d
    return mse + self.weight * coral, mse, coral


class WeightedMSELoss:
  def __init__(self, weight: float=2) -> None:
    self.weight = weight

  def forward(self, input: Tensor, target: Tensor):
    lt = (input < target).nonzero().squeeze(1)
    gt = (input > target).nonzero().squeeze(1)
    loss = 0
    if len(lt) > 0:
      lt_loss = F.mse_loss(
        input=torch.index_select(input,0,lt),
        target=torch.index_select(target,0,lt)
        )
      loss += lt_loss
    if len(gt) > 0:
      gt_loss = F.mse_loss(
        input=torch.index_select(input,0,gt),
        target=torch.index_select(target,0,gt)
      )
      loss += self.weight*gt_loss
    return loss

class PenaltyEnhancedMSELoss():
  def __init__(self, a: float=1, b:float=1) -> None:
    self.a = a
    self.b = b

  def forward(self, input: Tensor, target: Tensor):
    e = (input - target)**2
    e = (self.a + 1/(1 + torch.exp(-self.b * e))) * e
    loss = torch.sum(e) / len(e)
    return loss

LOSS = {
  "h": {
    "mse": MSELoss,
    "wmse": WeightedMSELoss,
    "pemse": PenaltyEnhancedMSELoss,
    "mmd": MMD,
    "coral": DeepCORAL,
  },
  "a": {
    "bce": BCEWithLogitsLoss,
  }
}

