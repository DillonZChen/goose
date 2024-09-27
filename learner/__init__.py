# This __init__.py file must be here as `learner` is a module loaded by NFD.
from .deep_learning.gnn_model import DeepLearningModel
from .feature_generation.wlf_model import FeatureGenerationModel


def print_torch_version():
    import torch
    print("PyTorch version:", torch.__version__, flush=True)
