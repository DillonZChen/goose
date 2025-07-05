import argparse
import logging
import os
from typing import Optional

import torch

from learning.predictor.neural_network.gnn import RGNN
from learning.predictor.neural_network.weights_dict import WeightsDict


def save_gnn_weights(save_file: str, weights_dict: Optional[WeightsDict]) -> None:
    if weights_dict is None:
        logging.critical("Trying to save empty model! Exiting save routine.")
        return
    save_dir = os.path.dirname(save_file)
    if len(save_dir) > 0:
        os.makedirs(save_dir, exist_ok=True)
    torch.save(weights_dict, save_file)


def load_gnn_weights(save_file: str) -> WeightsDict:
    if not os.path.exists(save_file):
        raise FileNotFoundError(f"Model file {save_file} does not exist.")

    if torch.cuda.is_available():
        weights_dict = torch.load(save_file, weights_only=False)
    else:
        weights_dict = torch.load(save_file, weights_only=False, map_location=torch.device("cpu"))

    return weights_dict
