from .loss import LOSS
from .mpnn import MPNNPredictor
from .elmpnn import ELMPNNPredictor

GNNS = {
    "MPNN": MPNNPredictor,
    "RGNN": ELMPNNPredictor,
}
