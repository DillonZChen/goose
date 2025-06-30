import logging
import os
from typing import Optional

import torch

from learning.predictor.neural_network.optimise import ModelDict


def save_gnn_weights(model_dict: Optional[ModelDict]) -> None:
    if model_dict is None:
        logging.critical("Trying to save empty model! Exiting save routine.")
    save_file = model_dict.opts.save_file
    save_dir = os.path.dirname(save_file)
    if len(save_dir) > 0:
        os.makedirs(save_dir, exist_ok=True)
    torch.save(model_dict, save_file)
    print(f"Saved gnn weights to {save_file}")
