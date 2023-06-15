from representation.base_class import *

class GDG_FEAT_MAP(Enum):
  PROPOPOSITION=0
  ACTION=1
  GOAL=2
  STATE=3
  PREDICATE=4
  SCHEMA=5

class GDG_EDGE_TYPES(Enum):
  PRE_EDGE=0
  ADD_EDGE=1
  DEL_EDGE=2
  PREDICATE=3


class GroundedDescriptionGraph(Representation, ABC):
  def __init__(self, domain_pddl: str, problem_pddl: str) -> None:
    super().__init__(domain_pddl, problem_pddl)

  def _init(self):
    self.rep_name = "gdg-el"
    self._FEAT_MAP = GDG_FEAT_MAP
    self.node_dim = len(self._FEAT_MAP)
    self.n_edge_types = len(GDG_EDGE_TYPES)
    self._compute_graph_representation()
    return

  def _compute_graph_representation(self) -> None:
    """
    Generates graph representation of a problem for input into the GNN. Given a state,
    we need to further concatenate binary values to indicate which propositions are set.
    """

    t = time.time()

    G = self._create_graph()

    grounded = explore(self.problem)
    propositions = set(grounded[1])
    actions = grounded[2]
    goals = set(str(p) for p in self.problem.goals)
    predicates = set()
    for p in grounded[1]:
      print(p.predicate)
    raise ValueError

    goal = 0

    # nodes
    for prop in propositions:
      p = str(prop)
      print(p)
      if p in goals:
        goal += 1
        x_p=self._one_hot_node(GDG_FEAT_MAP.GOAL.value)
      else:
        x_p=self._zero_node()  # will get replaced in state encoding

      G.add_node(p, x=x_p)
    for a in actions:
      print(a)
      G.add_node(a, x=self._one_hot_node(GDG_FEAT_MAP.ACTION.value))
    raise ValueError

    # edges
    for a in actions:
      for p in a.precondition:
        prop = str(p)
        # assert prop in G.nodes
        G.add_edge(u_of_edge=prop, v_of_edge=a, edge_type=GDG_EDGE_TYPES.PRE_EDGE.value)    # p_T -> a
      for p in a.add_effects:
        prop = str(p)
        # assert prop in G.nodes
        G.add_edge(u_of_edge=a,   v_of_edge=prop, edge_type=GDG_EDGE_TYPES.ADD_EDGE)  # a -> p_T
      for p in a.del_effects:
        prop = str(p)
        # assert prop in G.nodes
        G.add_edge(u_of_edge=a,   v_of_edge=prop, edge_type=GDG_EDGE_TYPES.DEL_EDGE)  # a -> p_F

    assert goal == len(goals)

    # map indices to nodes and vice versa
    self._node_to_i = {}
    for i, node in enumerate(G.nodes):
      self._node_to_i[node] = i

    # convert to PyG
    self._graph_to_el_representation(G)

    self._dump_stats(start_time=t)

    raise NotImplementedError

    return


  def get_state_enc(self, state: FrozenSet[Proposition]) -> Tuple[Tensor, Tensor]:

    x = self.x.clone()
    for p in state:
      if p not in self._node_to_i:
        continue
      x[self._node_to_i[p]][GDG_FEAT_MAP.STATE.value] = 1

    return x, self.edge_indices
