from .sdg import StripsProblemDescriptionGraph
from .sdg_el import EdgeLabeledStripsProblemDescriptionGraph

from .fdg import FdrProblemDescriptionGraph
from .fdg_el import EdgeLabeledFdrProblemDescriptionGraph

from .ldg import LiftedDescriptionGraph
from .ldg_el import EdgeLabaledLiftedDescriptionGraph

from .gdg_el import 

from .node_features import add_features

from .config import CONFIG


REPRESENTATIONS = {
  "sdg": StripsProblemDescriptionGraph,
  "sdg-el": EdgeLabeledStripsProblemDescriptionGraph,

  "fdg": FdrProblemDescriptionGraph,
  "fdg-el": EdgeLabeledFdrProblemDescriptionGraph,

  "ldg": LiftedDescriptionGraph,
  "ldg-el": EdgeLabaledLiftedDescriptionGraph,


}

N_EDGE_TYPES = {
    "sdg": 1,
    "sdg-el": 3,
    "fdg": 1,
    "fdg-el": 3,
    "ldg": 1,
    "ldg-el": 6,
}

