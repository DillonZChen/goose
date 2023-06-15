from representation.base_class import *

VAR = "var"
VAL = "val"
INIT = "init"
GOAL = "goal"
PRE = "pre"
EFF = "eff"

FDR_PDG_FEAT_MAP = {
  VAR: 0,
  VAL: 1,
  INIT: 2,
  GOAL: 3,
  PRE: 4,
  EFF: 5,
}


class FdrProblemDescriptionGraph(Representation, ABC):
  def __init__(self, domain_pddl: str, problem_pddl: str) -> None:
    super().__init__(domain_pddl, problem_pddl)

  def _init(self):
    self.rep_name = "fdg"
    self._FEAT_MAP = FDR_PDG_FEAT_MAP
    self.node_dim = len(self._FEAT_MAP)
    self._compute_graph_representation()
    return

  def _compute_graph_representation(self) -> None:

    t = time.time()

    G = self._create_graph()

    variables = {}
    goal = self.problem.goals
    goals = 0

    for var, val in self.problem.varval_to_fact:  # disgusting sas+ to strips under the hood
      if var not in variables:
        variables[var] = set()
      variables[var].add(val)

    # var val variables nodes and edges
    for var in variables:
      G.add_node(var, x=self._one_hot_node(self._FEAT_MAP[VAR]))
      for val in variables[var]:
        val_node = (var, val)
        val_x = self._one_hot_node(self._FEAT_MAP[VAL])

        if var in goal and val == goal[var]:
          goals += 1
          val_x += self._one_hot_node(self._FEAT_MAP[GOAL])

        G.add_node(val_node, x=val_x)

        # self.edge_groups[0].append((var, val_node))
        G.add_edge(u_of_edge=var, v_of_edge=val_node)
    assert goals == len(goal)

    # action nodes and edges
    for action in self.problem.actions:
      pre_node = (action.name, PRE)
      eff_node = (action.name, EFF)
      G.add_node(pre_node, x=self._one_hot_node(self._FEAT_MAP[PRE]))
      G.add_node(eff_node, x=self._one_hot_node(self._FEAT_MAP[EFF]))
      G.add_edge(u_of_edge=pre_node, v_of_edge=eff_node)
      for var, val in action.preconditions:
        assert val in variables[var]  # and hence should be in G.nodes()
        val_node = (var, val)
        assert val_node in G.nodes()
        G.add_edge(u_of_edge=pre_node, v_of_edge=val_node)

      for var, val in action.add_effects:  # from our compilation, effects are in add only
        assert val in variables[var]
        val_node = (var, val)
        assert val_node in G.nodes()
        G.add_edge(u_of_edge=eff_node, v_of_edge=val_node)

    # map fact to indices
    node_to_i = {}
    for i, node in enumerate(G.nodes):
      node_to_i[node] = i
    self.fact_to_i = {}
    for fact in self.problem.fact_to_varval:
      self.fact_to_i[fact] = node_to_i[self.problem.fact_to_varval[fact]]

    self.G = G
    pyg_G = from_networkx(G)
    self.x = pyg_G.x
    self.edge_index = pyg_G.edge_index

    self.num_nodes = len(G.nodes)
    self.num_edges = len(G.edges)

    self.graph_data = Data(x=self.x, edge_index=self.edge_index)
    self._dump_stats(start_time=t)

    return


  def get_state_enc(self, state: FrozenSet[Proposition]) -> Tuple[Tensor, Tensor]:
    x = self.x.clone()
    for p in state:
      if p not in self.problem.fact_to_varval:  
        # preprocess prunes some unnecessary facts
        continue
      x[self.fact_to_i[p]][self._FEAT_MAP[INIT]] = 1

    return x, self.edge_index
