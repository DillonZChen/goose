from .base_class import *
from planning.translate.pddl import Literal, Atom, NegatedAtom, PropositionalAction


class SLG_FEATURES(Enum):
  ACTION=0
  POSITIVE_GOAL=1
  NEGATIVE_GOAL=2
  STATE=3


class SLG_EDGE_LABELS(Enum):
  PRE_EDGE=0
  ADD_EDGE=1
  DEL_EDGE=2


""" Extended by GroundedLearningGraph and DeleteLearningGraph """
class StripsLearningGraph(Representation, ABC):
  name = "slg"
  n_node_features = len(SLG_FEATURES)
  n_edge_labels = len(SLG_EDGE_LABELS)
  directed = False
  lifted = False
  
  def __init__(self, domain_pddl: str, problem_pddl: str):
    super().__init__(domain_pddl, problem_pddl)

  def _get_grounded_problem_info(self):
    """ Ground the parsed lifted pddl representation and return 
        propositions, actions, positive and negative goals, and predicates.
        Predicates stores predicate names for both propositions and actions.

        This can be potentially optimised by letting the planner send the grounded information here.
    """

    # Grounding the lifted representation.
    grounded = explore(self.problem)

    propositions = set(grounded[1])
    actions = grounded[2]

    goals = self.problem.goal.parts if len(self.problem.goal.parts) > 0 else [self.problem.goal]
    positive_goals = set()
    negative_goals = set()
    for goal in goals:
      if type(goal) == Atom:
        positive_goals.add(goal)
      elif type(goal) == NegatedAtom:
        negative_goals.add(goal)
      else:
        raise TypeError(goal)
      
    predicates = set()
    for prop in propositions:
      predicates.add(self._get_predicate_from_proposition(prop))
    for action in actions:
      predicates.add(self._get_predicate_from_action(action))

    return propositions, actions, positive_goals, negative_goals, predicates
  
  def _get_predicate_from_proposition(self, proposition: Proposition) -> str:
    return proposition.predicate
  
  def _get_predicate_from_action(self, action: PropositionalAction) -> str:
    return action.name.replace("(","").replace(")","").split()[0]
  
  def _proposition_to_str(self, proposition: Literal) -> str:
    predicate = proposition.predicate
    args = proposition.args
    if len(args) == 0:
      return f"({predicate})"
    ret = f"({predicate}"
    for arg in args: ret += f" {arg}"
    ret += ")"
    return ret
  
  def _compute_graph_representation(self) -> None:
    """ TODO: reference definition of this graph representation
    """

    G = self._create_graph()

    propositions, actions, positive_goals, negative_goals, _ = self._get_grounded_problem_info()

    """ nodes """
    for proposition in propositions:
      node_p = self._proposition_to_str(proposition)
      # these features may get updated in state encoding
      if proposition in positive_goals:
        x_p = self._one_hot_node(SLG_FEATURES.POSITIVE_GOAL.value)
        self._pos_goal_nodes.add(node_p)
      elif proposition in negative_goals:
        x_p = self._one_hot_node(SLG_FEATURES.NEGATIVE_GOAL.value)
        self._neg_goal_nodes.add(node_p)
      else:
        x_p = self._zero_node()
      G.add_node(node_p, x=x_p)

    for action in actions:
      node_a = action.name
      x_a = self._one_hot_node(SLG_FEATURES.ACTION.value)
      G.add_node(node_a, x=x_a)

    """ edges """
    for action in actions:
      a_node = action.name
      for proposition in action.precondition:
        p_node = self._proposition_to_str(proposition)
        assert p_node in G.nodes, f"{p_node} not in nodes"
        assert a_node in G.nodes, f"{a_node} not in nodes"
        G.add_edge(u_of_edge=p_node, v_of_edge=a_node, edge_label=SLG_EDGE_LABELS.PRE_EDGE.value)
      for _, proposition in action.add_effects:  # ignoring conditional effects
        p_node = self._proposition_to_str(proposition)
        assert p_node in G.nodes, f"{p_node} not in nodes"
        assert a_node in G.nodes, f"{a_node} not in nodes"
        G.add_edge(u_of_edge=p_node, v_of_edge=a_node, edge_label=SLG_EDGE_LABELS.ADD_EDGE.value)
      for _, proposition in action.del_effects:  # ignoring conditional effects
        p_node = self._proposition_to_str(proposition)
        assert p_node in G.nodes, f"{p_node} not in nodes"
        assert a_node in G.nodes, f"{a_node} not in nodes"
        G.add_edge(u_of_edge=p_node, v_of_edge=a_node, edge_label=SLG_EDGE_LABELS.DEL_EDGE.value)

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
        x[self._node_to_i[p]][SLG_FEATURES.STATE.value] = 1

    return x, self.edge_indices

  def state_to_cgraph(self, state: State) -> CGraph:
    """ States are represented as a list of (pred, [args]) """
    c_graph = self.c_graph.copy()

    for p in state:

      # activated proposition overlaps with a goal Atom or NegatedAtom
      if p in self._pos_goal_nodes:
        c_graph.nodes[p]['colour'] = c_graph.nodes[p]['colour']+ACTIVATED_POS_GOAL_COLOUR_SUFFIX
      elif p in self._neg_goal_nodes:
        c_graph.nodes[p]['colour'] = c_graph.nodes[p]['colour']+ACTIVATED_NEG_GOAL_COLOUR_SUFFIX

    return c_graph
  