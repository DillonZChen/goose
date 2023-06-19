from planning.translate.pddl import Atom, NegatedAtom, Truth
from representation.base_class import *

class LDG_FEAT_MAP(Enum):
  P=0   # is predicate
  A=1   # is action
  G=2   # is goal (grounded)
  S=3   # is activated (grounded)
  N=4   # is NegatedAtom
  O=5   # is object
  PRE_POS = 6
  PRE_NEG = 7
  EFF_POS = 8
  EFF_NEG = 9

ENC_FEAT_SIZE = len(LDG_FEAT_MAP)
VAR_FEAT_SIZE = 4

class LiftedDescriptionGraph(Representation, ABC):
  def __init__(self, domain_pddl: str, problem_pddl: str) -> None:
    super().__init__(domain_pddl, problem_pddl)

  def _init(self):
    self.rep_name = "ldg"
    self._FEAT_MAP = LDG_FEAT_MAP
    self.node_dim = ENC_FEAT_SIZE+VAR_FEAT_SIZE
    self._construct_pe_function()
    return

  def _construct_pe_function(self):
    self._pe = []
    for idx in range(1000):  # precompute injective PE 
      torch.manual_seed(idx)
      rep = 2*torch.rand(VAR_FEAT_SIZE)-1  # U[-1,1]
      rep /= torch.linalg.norm(rep)
      self._pe.append(rep)
    return


  def _feature(self, node_type: LDG_FEAT_MAP):
    ret = torch.zeros(self.node_dim)
    ret[node_type.value] = 1  # faster
    return ret
  
  def _var_feature(self, idx: int):
    ret = torch.zeros(self.node_dim)
    ret[-VAR_FEAT_SIZE:] = self._pe[idx]
    return ret

  def _dump_task_stats(self) -> None:
    tqdm.write(f"Task stats:")
    tqdm.write(f"Domain name: {self.problem.domain_name}")
    tqdm.write(f"Task name: {self.problem.task_name}")
    tqdm.write(f"n_objects: {len(self.problem.objects)}")
    tqdm.write(f"n_predicates: {len(self.problem.predicates)}")
    tqdm.write(f"n_init: {len(self.problem.init)}")
    n_goals = len(self.problem.goal.parts)
    if n_goals == 0:
      n_goals = 1  # see Conjunction class
    tqdm.write(f"n_goal: {n_goals}")
    tqdm.write(f"n_actions: {len(self.problem.actions)}")
    pass

  def _compute_graph_representation(self) -> None:

    G = self._create_graph()

    # states have closed world assumption so never see NegatedAtom in inital state in Task object, but possible to see in goal condition

    # objects
    for i, obj in enumerate(self.problem.objects):
      G.add_node(obj.name, x=self._feature(LDG_FEAT_MAP.O))  # add object node


    # predicates
    largest_predicate = 0
    for pred in self.problem.predicates:
      largest_predicate = max(largest_predicate, len(pred.arguments))
      G.add_node(pred.name, x=self._feature(LDG_FEAT_MAP.P))  # add predicate node


    # fully connected between objects and predicates
    for pred in self.problem.predicates:
      for obj in self.problem.objects:
        G.add_edge(u_of_edge=pred.name, v_of_edge=obj.name)


    # goal (state gets dealt with below)
    if len(self.problem.goal.parts) == 0:
      goals = [self.problem.goal]
    else:
      goals = self.problem.goal.parts
    for z, fact in enumerate(goals):
      assert type(fact) in {Atom, NegatedAtom}

      # goal may have NegatedAtoms
      is_negated = type(fact)==NegatedAtom
      atom_desc = f"goal-{z}-{int(not is_negated)}"

      pred = fact.predicate
      args = fact.args
      pred_node = (pred, atom_desc)

      x = self._feature(LDG_FEAT_MAP.G)
      if is_negated: x += self._feature(LDG_FEAT_MAP.N)
      G.add_node(pred_node, x=x)  # add grounded predicate node

      for i, arg in enumerate(args):
        pred_var_node = (pred, f"{atom_desc}-var-{i}")  # pred var node
        G.add_node(pred_var_node, x=self._var_feature(idx=i))

        # connect variable to predicate
        G.add_edge(u_of_edge=pred_node, v_of_edge=pred_var_node)

        # connect variable to object
        assert arg in G.nodes()
        G.add_edge(u_of_edge=pred_var_node, v_of_edge=arg)

      # connect grounded fact to predicate
      assert pred in G.nodes()
      G.add_edge(u_of_edge=pred_node, v_of_edge=pred)
    # end goal


    # actions
    largest_action_schema = 0
    for action in self.problem.actions:
      G.add_node(action.name, x=self._feature(LDG_FEAT_MAP.A))
      action_args = {}

      largest_action_schema = max(largest_action_schema, len(action.parameters))
      for i, arg in enumerate(action.parameters):
        arg_node = (action.name, f"action-var-{i}")  # action var
        G.add_node(arg_node, x=self._var_feature(idx=i))
        action_args[arg.name] = arg_node
        G.add_edge(u_of_edge=action.name, v_of_edge=arg_node)

      def deal_with_action_prec_or_eff(predicates, edge_type):
        for z, predicate in enumerate(predicates):
          pred = predicate.predicate
          aux_node = (pred, f"{edge_type}-aux-{z}")  # aux node for duplicate preds
          G.add_node(aux_node, x=self._feature(edge_type))

          assert pred in G.nodes()
          G.add_edge(u_of_edge=pred, v_of_edge=aux_node)

          if len(predicate.args) > 0:
            for j, arg in enumerate(predicate.args):
              prec_arg_node = (arg, f"{edge_type}-aux-{z}-var-{j}")  # aux var
              G.add_node(prec_arg_node, x=self._var_feature(idx=j))
              G.add_edge(u_of_edge=aux_node, v_of_edge=prec_arg_node)

              if arg in action_args: 
                # TODO deal with cases like 
                # childsnack-contents-parsizeP-chamC
                action_arg_node = action_args[arg]
                G.add_edge(u_of_edge=prec_arg_node, v_of_edge=action_arg_node)
              # action_arg_node = action_args[arg]
              # G.add_edge(u_of_edge=prec_arg_node, v_of_edge=action_arg_node)
          else:  # unitary predicate so connect directly to action
              G.add_edge(u_of_edge=aux_node, v_of_edge=action.name)
        return

      pos_pres = [p for p in action.precondition.parts if type(p)==Atom]
      neg_pres = [p for p in action.precondition.parts if type(p)==NegatedAtom]
      pos_effs = [p.literal for p in action.effects if type(p.literal)==Atom]
      neg_effs = [p.literal for p in action.effects if type(p.literal)==NegatedAtom]
      for p in action.effects:
        assert type(p.condition) == Truth  # no conditional effects
      deal_with_action_prec_or_eff(pos_pres, LDG_FEAT_MAP.PRE_POS)
      deal_with_action_prec_or_eff(neg_pres, LDG_FEAT_MAP.PRE_NEG)
      deal_with_action_prec_or_eff(pos_effs, LDG_FEAT_MAP.EFF_POS)
      deal_with_action_prec_or_eff(neg_effs, LDG_FEAT_MAP.EFF_NEG)
    # end actions

    assert largest_predicate > 0
    assert largest_action_schema > 0

    # map indices to nodes and vice versa
    self._node_to_i = {}
    for i, node in enumerate(G.nodes):
      self._node_to_i[node] = i

    # convert to PyG
    self._graph_to_representation(G)

    return



  def get_state_enc(self, state: List[Tuple[str, List[str]]]) -> Tuple[Tensor, Tensor]:

    x = self.x.clone()
    edge_index = self.edge_index.clone()
    i = len(x)

    to_add = sum(len(fact[1])+1 for fact in state)
    x = torch.nn.functional.pad(x, (0, 0, 0, to_add), "constant", 0)
    append_edge_index = []

    for fact in state:
      pred = fact[0]
      args = fact[1]
      if pred not in self._node_to_i:
        continue  # e.g. type predicates

      pred_node_i = i
      x[i][LDG_FEAT_MAP.S.value] = 1
      i += 1

      for k, arg in enumerate(args):
        pred_var_node_i = i
        x[i][-VAR_FEAT_SIZE:] = self._pe[k]
        i += 1

        # connect variable to predicate
        append_edge_index.append((pred_node_i, pred_var_node_i))
        append_edge_index.append((pred_var_node_i, pred_node_i))

        # connect variable to object
        append_edge_index.append((pred_var_node_i, self._node_to_i[arg]))
        append_edge_index.append((self._node_to_i[arg], pred_var_node_i))

      # connect grounded fact to predicate
      append_edge_index.append((pred_node_i, self._node_to_i[pred]))
      append_edge_index.append((self._node_to_i[pred], pred_node_i))

    edge_index = torch.hstack((edge_index, torch.tensor(append_edge_index).T)).long()
    
    return x, edge_index
  