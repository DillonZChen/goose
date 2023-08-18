from representation.base_class import *
from representation.slg import StripsLearningGraph


class GLG_FEATURES(Enum):
  ACTION=0
  POSITIVE_GOAL=1
  NEGATIVE_GOAL=2
  STATE=3
  PREDICATE=4
  SCHEMA=5


class GLG_EDGE_TYPES(Enum):
  PRE_EDGE=0
  ADD_EDGE=1
  DEL_EDGE=2
  PREDICATE=3


class GroundedLearningGraph(StripsLearningGraph, ABC):
  name = "glg"
  n_node_features = len(GLG_FEATURES)
  n_edge_labels = len(GLG_EDGE_TYPES)
  directed = False
  lifted = False
  
  def __init__(self, domain_pddl: str, problem_pddl: str):
    super().__init__(domain_pddl, problem_pddl)

  def _compute_graph_representation(self) -> None:
    """ TODO: reference definition of this graph representation (not used in 24-AAAI paper)
    """

    G = self._create_graph()

    propositions, actions, positive_goals, negative_goals, predicates = self._get_grounded_problem_info()
    
    """ nodes """
    for proposition in propositions:
      node_p = self._proposition_to_str(proposition)
      # these features may get updated in state encoding
      if proposition in positive_goals:
        x_p=self._one_hot_node(GLG_FEATURES.POSITIVE_GOAL.value)
      elif proposition in negative_goals:
        x_p=self._one_hot_node(GLG_FEATURES.NEGATIVE_GOAL.value)
      else:
        x_p=self._zero_node()
      G.add_node(node_p, x=x_p)

    for action in actions:
      node_a = action.name
      x_a = self._one_hot_node(GLG_FEATURES.ACTION.value)
      G.add_node(node_a, x=x_a)

    for predicate in predicates:
      node_pred = predicate
      x_pred = self._one_hot_node(GLG_FEATURES.PREDICATE.value)
      G.add_node(node_pred, x=x_pred)

    """ edges """
    for action in actions:
      # edges between actions and schema
      a_node = action.name
      s_node = self._get_predicate_from_action(action)
      assert a_node in G.nodes
      assert s_node in G.nodes
      G.add_edge(u_of_edge=a_node, v_of_edge=s_node, edge_type=GLG_EDGE_TYPES.PREDICATE.value)

      # edges between actions and propositions
      for proposition in action.precondition:
        p_node = self._proposition_to_str(proposition)
        assert p_node in G.nodes, f"{p_node} not in nodes"
        assert a_node in G.nodes, f"{a_node} not in nodes"
        G.add_edge(u_of_edge=p_node, v_of_edge=a_node, edge_type=GLG_EDGE_TYPES.PRE_EDGE.value)
      for _, proposition in action.add_effects:  # ignoring conditional effects
        p_node = self._proposition_to_str(proposition)
        assert p_node in G.nodes, f"{p_node} not in nodes"
        assert a_node in G.nodes, f"{a_node} not in nodes"
        G.add_edge(u_of_edge=p_node, v_of_edge=a_node, edge_type=GLG_EDGE_TYPES.ADD_EDGE.value)
      for _, proposition in action.del_effects:  # ignoring conditional effects
        p_node = self._proposition_to_str(proposition)
        assert p_node in G.nodes, f"{p_node} not in nodes"
        assert a_node in G.nodes, f"{a_node} not in nodes"
        G.add_edge(u_of_edge=p_node, v_of_edge=a_node, edge_type=GLG_EDGE_TYPES.DEL_EDGE.value)

    for proposition in propositions:
      # edge between propositions and predicates
      p_node = self._proposition_to_str(proposition)
      pred_node = self._get_predicate_from_proposition(proposition)
      assert p_node in G.nodes
      assert pred_node in G.nodes
      G.add_edge(u_of_edge=p_node, v_of_edge=pred_node, edge_type=GLG_EDGE_TYPES.PREDICATE.value)

    # map node name to index
    self._node_to_i = {}
    for i, node in enumerate(G.nodes):
      self._node_to_i[node] = i
    self.G = G

    return

  def state_to_tensor(self, state: State) -> Tuple[Tensor, Tensor]:

    x = self.x.clone()  # not time nor memory efficient, but no other way in Python
    for p in state:
      if p in self._node_to_i:
        x[self._node_to_i[p]][GLG_FEATURES.STATE.value] = 1

    return x, self.edge_indices
