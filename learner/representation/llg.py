from planning.translate.pddl import Atom, NegatedAtom, Truth
from representation.base_class import *

class LLG_FEATURES(Enum):
  P=0   # is predicate
  A=1   # is action
  G=2   # is positive goal (grounded)
  N=3   # is negative goal (grounded)
  S=4   # is activated (grounded)
  O=5   # is object


ENC_FEAT_SIZE = len(LLG_FEATURES)
VAR_FEAT_SIZE = 4


LLG_EDGE_TYPES = OrderedDict({
  "neutral": 0,
  "ground":  1,
  "pre_pos": 2,
  "pre_neg": 3,
  "eff_pos": 4,
  "eff_neg": 5,
})
  


class LiftedLearningGraph(Representation, ABC):
  name = "llg"
  n_node_features = ENC_FEAT_SIZE+VAR_FEAT_SIZE
  n_edge_labels = len(LLG_EDGE_TYPES)
  directed = False
  lifted = True
  
  def __init__(self, domain_pddl: str, problem_pddl: str):
    super().__init__(domain_pddl, problem_pddl)

  def _construct_if(self) -> None:
    """ Precompute a seeded randomly generated injective index function """
    self._pe = []

    # TODO read max range from problem and lazily compute 
    for idx in range(60):
      torch.manual_seed(idx)
      rep = 2*torch.rand(VAR_FEAT_SIZE)-1  # U[-1,1]
      rep /= torch.linalg.norm(rep)
      self._pe.append(rep)
    return

  def _feature(self, node_type: LLG_FEATURES) -> Tensor:
    ret = torch.zeros(self.n_node_features)
    ret[node_type.value] = 1
    return ret
  
  def _if_feature(self, idx: int) -> Tensor:
    ret = torch.zeros(self.n_node_features)
    ret[-VAR_FEAT_SIZE:] = self._pe[idx]
    return ret

  def _compute_graph_representation(self) -> None:
    """ TODO: reference definition of this graph representation
    """
  
    self._construct_if()

    G = self._create_graph()

    # objects
    for i, obj in enumerate(self.problem.objects):
      G.add_node(obj.name, x=self._feature(LLG_FEATURES.O))  # add object node


    # predicates
    largest_predicate = 0
    for pred in self.problem.predicates:
      largest_predicate = max(largest_predicate, len(pred.arguments))
      G.add_node(pred.name, x=self._feature(LLG_FEATURES.P))  # add predicate node


    # fully connected between objects and predicates
    for pred in self.problem.predicates:
      for obj in self.problem.objects:
        G.add_edge(u_of_edge=pred.name, v_of_edge=obj.name, edge_type=LLG_EDGE_TYPES["neutral"])


    # goal (state gets dealt with in get_state_enc)
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
      else:
        x = self._feature(LLG_FEATURES.G)
      G.add_node(goal_node, x=x)  # add grounded predicate node

      for i, arg in enumerate(args):
        goal_var_node = (goal_node, i)
        G.add_node(goal_var_node, x=self._if_feature(idx=i))

        # connect variable to predicate
        G.add_edge(u_of_edge=goal_node, v_of_edge=goal_var_node, edge_type=LLG_EDGE_TYPES["ground"])

        # connect variable to object
        assert arg in G.nodes()
        G.add_edge(u_of_edge=goal_var_node, v_of_edge=arg, edge_type=LLG_EDGE_TYPES["ground"])

      # connect grounded fact to predicate
      assert pred in G.nodes()
      G.add_edge(u_of_edge=goal_node, v_of_edge=pred, edge_type=LLG_EDGE_TYPES["ground"])
    # end goal


    # actions
    largest_action_schema = 0
    for action in self.problem.actions:
      G.add_node(action.name, x=self._feature(LLG_FEATURES.A))
      action_args = {}

      largest_action_schema = max(largest_action_schema, len(action.parameters))
      for i, arg in enumerate(action.parameters):
        arg_node = (action.name, f"action-var-{i}")  # action var
        G.add_node(arg_node, x=self._if_feature(idx=i))
        action_args[arg.name] = arg_node
        G.add_edge(u_of_edge=action.name, v_of_edge=arg_node, edge_type=LLG_EDGE_TYPES["neutral"])

      def deal_with_action_prec_or_eff(predicates, edge_type):
        for z, predicate in enumerate(predicates):
          pred = predicate.predicate
          aux_node = (pred, f"{edge_type}-aux-{z}")  # aux node for duplicate preds
          G.add_node(aux_node, x=self._zero_node())

          assert pred in G.nodes()
          G.add_edge(u_of_edge=pred, v_of_edge=aux_node, edge_type=LLG_EDGE_TYPES[edge_type])

          if len(predicate.args) > 0:
            for j, arg in enumerate(predicate.args):
              prec_arg_node = (arg, f"{edge_type}-aux-{z}-var-{j}")  # aux var
              G.add_node(prec_arg_node, x=self._if_feature(idx=j))
              G.add_edge(u_of_edge=aux_node, v_of_edge=prec_arg_node, edge_type=LLG_EDGE_TYPES[edge_type])

              if arg in action_args: 
                action_arg_node = action_args[arg]
                G.add_edge(u_of_edge=prec_arg_node, v_of_edge=action_arg_node, edge_type=LLG_EDGE_TYPES[edge_type])
          else:  # unitary predicate so connect directly to action
              G.add_edge(u_of_edge=aux_node, v_of_edge=action.name, edge_type=LLG_EDGE_TYPES[edge_type])
        return

      pos_pres = [p for p in action.precondition.parts if type(p)==Atom]
      neg_pres = [p for p in action.precondition.parts if type(p)==NegatedAtom]
      pos_effs = [p.literal for p in action.effects if type(p.literal)==Atom]
      neg_effs = [p.literal for p in action.effects if type(p.literal)==NegatedAtom]
      for p in action.effects:
        assert type(p.condition) == Truth  # no conditional effects
      deal_with_action_prec_or_eff(pos_pres, "pre_pos")
      deal_with_action_prec_or_eff(neg_pres, "pre_neg")
      deal_with_action_prec_or_eff(pos_effs, "eff_pos")
      deal_with_action_prec_or_eff(neg_effs, "eff_neg")
    # end actions

    assert largest_predicate > 0
    assert largest_action_schema > 0

    # map indices to nodes and vice versa
    # can be optimised by only saving a subset of nodes
    self._node_to_i = {}
    for i, node in enumerate(G.nodes):
      self._node_to_i[node] = i

    # convert to PyG
    self._graph_to_representation(G)

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

  def get_state_enc(self, state: List[Tuple[str, List[str]]]) -> Tuple[Tensor, Tensor]:
    """ States are represented as a list of (pred, [args]) """
    x = self.x.clone()
    edge_indices = self.edge_indices.copy()
    i = len(x)

    to_add = sum(len(fact[1])+1 for fact in state)
    x = torch.nn.functional.pad(x, (0, 0, 0, to_add), "constant", 0)
    append_edge_index = []

    for fact in state:
      pred = fact[0]
      args = fact[1]

      node = (pred, tuple(args))

      # activated proposition overlaps with a goal Atom or NegatedAtom
      if node in self._node_to_i:
        x[self._node_to_i[node]][LLG_FEATURES.S.value] = 1
        continue
      
      # activated proposition does not overlap with a goal
      true_node_i = i
      x[i][LLG_FEATURES.S.value] = 1
      i += 1

      # connect fact to predicate
      append_edge_index.append((true_node_i, self._node_to_i[pred]))
      append_edge_index.append((self._node_to_i[pred], true_node_i))

      # connect to predicates and objects
      for k, arg in enumerate(args):
        true_var_node_i = i
        x[i][-VAR_FEAT_SIZE:] = self._pe[k]
        i += 1

        # connect variable to predicate
        append_edge_index.append((true_node_i, true_var_node_i))
        append_edge_index.append((true_var_node_i, true_node_i))

        # connect variable to object
        append_edge_index.append((true_var_node_i, self._node_to_i[arg]))
        append_edge_index.append((self._node_to_i[arg], true_var_node_i))

    edge_indices[LLG_EDGE_TYPES["ground"]] = torch.hstack((edge_indices[LLG_EDGE_TYPES["ground"]], 
                                                      torch.tensor(append_edge_index).T)).long()
    
    return x, edge_indices
  