from dataclasses import dataclass

import torch


@dataclass
class WeightsDict:
    weights: dict[str, torch.Tensor]
    epoch: int
    train_loss: float
    val_loss: float
