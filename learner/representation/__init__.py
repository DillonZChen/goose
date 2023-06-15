from .sdg import StripsProblemDescriptionGraph
from .sdg_el import EdgeLabeledStripsProblemDescriptionGraph

from .fdg import FdrProblemDescriptionGraph
from .fdg_el import EdgeLabeledFdrProblemDescriptionGraph

from .ldg import LiftedDescriptionGraph
from .ldg_el import EdgeLabaledLiftedDescriptionGraph

from .gdg_el import GroundedDescriptionGraph

from .node_features import add_features

from .config import CONFIG, N_EDGE_TYPES


REPRESENTATIONS = {
  "sdg": StripsProblemDescriptionGraph,
  "sdg-el": EdgeLabeledStripsProblemDescriptionGraph,

  "fdg": FdrProblemDescriptionGraph,
  "fdg-el": EdgeLabeledFdrProblemDescriptionGraph,

  "ldg": LiftedDescriptionGraph,
  "ldg-el": EdgeLabaledLiftedDescriptionGraph,

  "gdg-el": GroundedDescriptionGraph,
}

