from representation.base_class import *


EL_STRIPS_PDG_FEAT_MAP = {
  "p": 0,   # prop
  "a": 1,   # action
  "g": 2,   # goal
  "s": 3,   # initial state
}

PRE_EDGE=0
ADD_EDGE=1
DEL_EDGE=2


class EdgeLabelledStripsProblemDescriptionGraph(Representation, ABC):
  def __init__(self, domain_pddl: str, problem_pddl: str) -> None:
    super().__init__(domain_pddl, problem_pddl)

  def _init(self):
    self.rep_name = "sdg-el"
    self._FEAT_MAP = EL_STRIPS_PDG_FEAT_MAP
    self.node_dim = len(self._FEAT_MAP)
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
    goals = set(str(p) for p in self.problem.goal.parts)

    goal = 0

    # nodes
    for p in propositions:
      if p in goals:
        goal += 1
        x_p=self._one_hot_node(self._FEAT_MAP['g'])
      else:
        x_p=self._zero_node()  # will get replaced in state encoding

      G.add_node(p,   x=x_p)
    for a in actions:
      G.add_node(a,   x=self._one_hot_node(self._FEAT_MAP['a']))

    # edges
    for a in actions:
      for p in a.precondition:
        assert str(p) in G.nodes, f"{str(p)} not in nodes"
        assert a in G.nodes
        G.add_edge(u_of_edge=a, v_of_edge=str(p), edge_type=PRE_EDGE)
      for p in a.add_effects:
        assert str(p) in G.nodes, f"{str(p)} not in nodes"
        assert a in G.nodes
        G.add_edge(u_of_edge=a, v_of_edge=str(p), edge_type=ADD_EDGE)
      for p in a.del_effects:
        assert str(p) in G.nodes, f"{str(p)} not in nodes"
        assert a in G.nodes
        G.add_edge(u_of_edge=a, v_of_edge=str(p), edge_type=DEL_EDGE)

    assert goal == len(goals)

    # map indices to nodes and vice versa
    self._node_to_i = {}
    for i, node in enumerate(G.nodes):
      if node not in propositions:
        continue
      self._node_to_i[node] = i

    # convert to PyG
    self._graph_to_representation(G)
    self._dump_stats(start_time=t)
    return


  def get_state_enc(self, state: FrozenSet[Proposition]) -> Tuple[Tensor, Tensor]:
    x = self.x.clone()
    for p in state:
      if p not in self._node_to_i:
        continue
      x[self._node_to_i[p]][self._FEAT_MAP['s']] = 1

    return x, self.edge_indices
