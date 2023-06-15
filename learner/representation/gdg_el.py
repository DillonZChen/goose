from representation.base_class import *

class GDG_FEAT_MAP(Enum):
  PROPOPOSITION=0
  ACTION=1
  GOAL=2
  STATE=3
  PREDICATE=4
  SCHEMA=5

class GDG_EDGE_TYPES(Enum):
  PRECONDITION=0
  ADD_EFFECT=1
  DEL_EFFECT=2
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
    propositions = set(str(p) for p in grounded[1])
    actions = grounded[2]
    goals = set(str(p) for p in self.problem.goals)

    goal = 0

    # nodes
    for p in propositions:
      if p in goals:
        goal += 1
        x_p=self._one_hot_node(self._FEAT_MAP['g'])
      else:
        x_p=self._zero_node()  # will get replaced in state encoding

      G.add_node(p,   x=x_p)
      G.add_node(p+T, x=self._one_hot_node(self._FEAT_MAP[T]))
      G.add_node(p+F, x=self._one_hot_node(self._FEAT_MAP[F]))
    for a in actions:
      G.add_node(a,   x=self._one_hot_node(self._FEAT_MAP['a']))

    # edges
    for p in propositions:
      G.add_edge(u_of_edge=p,   v_of_edge=p+T)    # p -> p_T
      G.add_edge(u_of_edge=p,   v_of_edge=p+F)    # p -> p_F
      G.add_edge(u_of_edge=p+F, v_of_edge=p+T)    # p_F -> p_T  # communicate when node is off
    for a in actions:
      for p in a.precondition:
        prop = str(p)
        # assert prop in G.nodes
        G.add_edge(u_of_edge=prop+T, v_of_edge=a)    # p_T -> a
      for p in a.add_effects:
        prop = str(p)
        # assert prop in G.nodes
        G.add_edge(u_of_edge=a,   v_of_edge=prop+T)  # a -> p_T
      for p in a.del_effects:
        prop = str(p)
        # assert prop in G.nodes
        G.add_edge(u_of_edge=a,   v_of_edge=prop+F)  # a -> p_F

    assert goal == len(goals)

    self.G = G
    self.num_nodes = len(G.nodes)
    self.num_edges = len(G.edges)

    # map indices to nodes and vice versa
    self._node_to_i = {}
    self._i_to_node = {}
    for i, node in enumerate(G.nodes):
      if node not in propositions:
        continue
      self._node_to_i[node] = i
      self._i_to_node[i] = node

    pyg_G = from_networkx(G)
    self.x = pyg_G.x
    self.edge_index = pyg_G.edge_index

    self.graph_data = Data(x=self.x, edge_index=self.edge_index)
    self._dump_stats(start_time=t)

    raise NotImplementedError

    return


  def get_state_enc(self, state: FrozenSet[Proposition]) -> Tuple[Tensor, Tensor]:

    x = self.x.clone()
    for p in state:
      if p not in self._node_to_i:
        continue
      # unlike drg, ok to have both [s] and [g] activated
      # only p nodes are stored in _node_index
      x[self._node_to_i[p]][self._FEAT_MAP['s']] = 1

    raise NotImplementedError

    return x, self.edge_index
