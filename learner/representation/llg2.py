from .base_class import *
from planning.translate.pddl import Atom, NegatedAtom, Truth


class LLG_FEATURES(Enum):
  P=0   # is predicate
  G=1   # is positive goal (grounded)
  N=2   # is negative goal (grounded)
  O=3   # is object
  S=4   # is activated (grounded)


ENC_FEAT_SIZE = len(LLG_FEATURES)


class LiftedLearningGraph2(Representation, ABC):
  name = "llg"
  n_node_features = ENC_FEAT_SIZE
  n_edge_labels = float("inf")  # unbounded because of var size
  directed = False
  lifted = True
  
  def __init__(self, domain_pddl: str, problem_pddl: str):
    super().__init__(domain_pddl, problem_pddl)

  def _feature(self, node_type: LLG_FEATURES) -> Tensor:
    ret = torch.zeros(self.n_node_features)
    ret[node_type.value] = 1
    return ret

  def _compute_graph_representation(self) -> None:
    """ TODO: reference definition of this graph representation
    """
  
    G = self._create_graph()

    # objects
    for i, obj in enumerate(self.problem.objects):
      G.add_node(obj.name, x=self._feature(LLG_FEATURES.O))  # add object node


    # predicates
    largest_predicate = 0
    for pred in self.problem.predicates:
      largest_predicate = max(largest_predicate, len(pred.arguments))
      G.add_node(pred.name, x=self._feature(LLG_FEATURES.P))  # add predicate node

    # goal (state gets dealt with in state_to_tensor)
    if len(self.problem.goal.parts) == 0:
      goals = [self.problem.goal]
    else:
      goals = self.problem.goal.parts
    for fact in goals:
      assert type(fact) in {Atom, NegatedAtom}

      # may have negative goals
      is_negated = type(fact)==NegatedAtom

      pred = fact.predicate
      args = fact.args
      goal_node = (pred, args)

      if is_negated: 
        x = self._feature(LLG_FEATURES.N)
        self._neg_goal_nodes.add(goal_node)
      else:          
        x = self._feature(LLG_FEATURES.G)
        self._pos_goal_nodes.add(goal_node)
      G.add_node(goal_node, x=x)  # add fact node

      # connect fact to predicate
      assert pred in G.nodes()
      G.add_edge(u_of_edge=goal_node, v_of_edge=pred, edge_label=-1)
      G.add_edge(v_of_edge=goal_node, u_of_edge=pred, edge_label=-1)

      for k, arg in enumerate(args):
        # connect fact to object
        G.add_edge(u_of_edge=goal_node, v_of_edge=arg, edge_label=k)
        G.add_edge(v_of_edge=goal_node, u_of_edge=arg, edge_label=k)
    # end goal

    assert largest_predicate > 0

    # map node name to index
    self._node_to_i = {}
    for i, node in enumerate(G.nodes):
      self._node_to_i[node] = i
    self.G = G

    return

  def str_to_state(self, s) -> List[Tuple[str, List[str]]]:
    """ Used in dataset construction to convert string representation of facts into a (pred, [args]) representation """
    state = []
    for fact in s:
      fact = fact.replace(")", "").replace("(", "")
      toks = fact.split()
      if toks[0] == "=":
        continue
      if len(toks) > 1:
        state.append((toks[0], toks[1:]))
      else:
        state.append((toks[0], ()))
    return state

  def state_to_tensor(self, state: List[Tuple[str, List[str]]]) -> TGraph:
    raise NotImplementedError
  
  def state_to_cgraph(self, state: List[Tuple[str, List[str]]]) -> CGraph:
    """ States are represented as a list of (pred, [args]) """
    c_graph = self.c_graph.copy()
    new_idx = len(self._name_to_node)

    for fact in state:
      pred = fact[0]
      args = fact[1]

      if len(pred)==0:
        continue

      node = (pred, tuple(args))

      # activated proposition overlaps with a goal Atom or NegatedAtom
      if node in self._pos_goal_nodes:
        idx = self._name_to_node[node]
        c_graph.nodes[idx]['colour'] = ACTIVATED_POS_GOAL_COLOUR
        # print(node, idx)
        continue
      elif node in self._neg_goal_nodes:
        idx = self._name_to_node[node]
        c_graph.nodes[idx]['colour'] = ACTIVATED_NEG_GOAL_COLOUR
        continue

      node = new_idx
      new_idx += 1

      # else add node and corresponding edges to graph
      c_graph.add_node(node, colour=ACTIVATED_COLOUR)

      # connect fact to predicate
      assert self._name_to_node[pred] in c_graph.nodes
      c_graph.add_edge(u_of_edge=node, v_of_edge=self._name_to_node[pred], edge_label=-1)
      c_graph.add_edge(v_of_edge=node, u_of_edge=self._name_to_node[pred], edge_label=-1)

      for k, obj in enumerate(args):

        # connect fact to object
        assert self._name_to_node[obj] in c_graph.nodes
        c_graph.add_edge(u_of_edge=node, v_of_edge=self._name_to_node[obj], edge_label=k)
        c_graph.add_edge(v_of_edge=node, u_of_edge=self._name_to_node[obj], edge_label=k)
        
    return c_graph
  