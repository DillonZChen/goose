from .base_class import Representation, CGraph, TGraph
from .slg import StripsLearningGraph
from .flg import FdrLearningGraph
from .llg import LiftedLearningGraph
from .ilg import InstanceLearningGraph
from .opg import ObjectPairGraph
from .planning import State


REPRESENTATIONS = {
    "slg": StripsLearningGraph,
    "flg": FdrLearningGraph,
    "llg": LiftedLearningGraph,
    "ilg": InstanceLearningGraph,
    "opg": ObjectPairGraph,
}
