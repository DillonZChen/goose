from .base_class import CGraph, TGraph, Representation
from .slg import StripsLearningGraph
from .dlg import DeleteLearningGraph
from .flg import FdrLearningGraph
from .llg import LiftedLearningGraph
from .llg2 import LiftedLearningGraph2
from .glg import GroundedLearningGraph


REPRESENTATIONS = {
  "slg": StripsLearningGraph,
  "dlg": DeleteLearningGraph,
  "flg": FdrLearningGraph,
  "llg": LiftedLearningGraph2,
  "llg-old": LiftedLearningGraph,
  "glg": GroundedLearningGraph,
}

