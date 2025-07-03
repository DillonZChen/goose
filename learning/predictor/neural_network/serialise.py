import argparse
import logging
import os
from typing import Optional

import torch

from learning.predictor.neural_network.gnn import RGNN
from learning.predictor.neural_network.optimise import WeightsDict


def save_gnn_weights(save_file: str, weights_dict: Optional[WeightsDict]) -> None:
    if weights_dict is None:
        logging.critical("Trying to save empty model! Exiting save routine.")
        return
    save_dir = os.path.dirname(save_file)
    if len(save_dir) > 0:
        os.makedirs(save_dir, exist_ok=True)
    torch.save(weights_dict, save_file)
    print(f"Saved gnn weights to {save_file}")


def load_gnn_weights(save_file: str) -> WeightsDict:
    if not os.path.exists(save_file):
        raise FileNotFoundError(f"Model file {save_file} does not exist.")

    if torch.cuda.is_available():
        weights_dict = torch.load(save_file, weights_only=False)
    else:
        weights_dict = torch.load(save_file, weights_only=False, map_location=torch.device("cpu"))

    if not isinstance(weights_dict, WeightsDict):
        raise TypeError(f"Expected ModelDict, got {type(weights_dict)}")
    return weights_dict


def load_gnn(save_file: str) -> tuple[RGNN, argparse.Namespace]:
    weights_dict = load_gnn_weights(save_file)
    opts = weights_dict.opts
    weights = weights_dict.weights
    model = RGNN.init_from_opts(opts=opts)
    model.load_state_dict(weights)
    return model, opts
