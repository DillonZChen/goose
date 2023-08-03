from .sdg_el import EdgeLabelledStripsProblemDescriptionGraph
from .ddg_el import DeleteLearningGraph
from .fdg_el import EdgeLabelledFdrProblemDescriptionGraph
from .ldg_el import EdgeLabelledLiftedDescriptionGraph
from .gdg_el import EdgeLabelledGroundedDescriptionGraph

from .node_features import add_features

from .config import CONFIG, N_EDGE_TYPES


REPRESENTATIONS = {
  "sdg-el": EdgeLabelledStripsProblemDescriptionGraph,
  "ddg-el": DeleteLearningGraph,
  "fdg-el": EdgeLabelledFdrProblemDescriptionGraph,
  "ldg-el": EdgeLabelledLiftedDescriptionGraph,
  "gdg-el": EdgeLabelledGroundedDescriptionGraph,
}

