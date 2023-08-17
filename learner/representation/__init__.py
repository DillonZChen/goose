from .slg import StripsLearningGraph
from .dlg import DeleteLearningGraph
from .flg import FdrLearningGraph
from .llg import LiftedLearningGraph
from .glg import GroundedLearningGraph
from .node_features import add_features


REPRESENTATIONS = {
  "slg": StripsLearningGraph,
  "dlg": DeleteLearningGraph,
  "flg": FdrLearningGraph,
  "llg": LiftedLearningGraph,
  "glg": GroundedLearningGraph,
}

