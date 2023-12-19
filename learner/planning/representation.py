import os
import planning
import random
from datetime import datetime
from typing import FrozenSet, TypeVar
from planning.translate.pddl import Task

Proposition = TypeVar("Proposition", bound=str)
State = FrozenSet[Proposition]


class Operator():
  def __init__(self, name, preconditions, add_effects, del_effects):
    self.name = name
    self.preconditions = preconditions
    self.add_effects = add_effects
    self.del_effects = del_effects


class FDRProblem():
  def __init__(self, domain_pddl, problem_pddl):
    """ Very hacky way to construct an FDR problem object from the downward translator """

    dt = datetime.now().strftime("%d_%m_%Y_%H_%M_%S")
    sas_file = f"sas_file_output_{dt}_{domain_pddl.replace('/','')}_{problem_pddl.replace('/','')}.sas"

    if os.path.exists("planning/downward/fast-downward.py"):
      cmd = f"./planning/downward/fast-downward.py --translate --sas-file {sas_file} {domain_pddl} {problem_pddl}"
    else:
      script= os.path.expandvars(f"${{GOOSE}}/planning/downward/fast-downward.py")
      assert os.path.exists(script)
      cmd = f"python3 {script} --translate --sas-file {sas_file} {domain_pddl} {problem_pddl}"
    
    # avoid lots of text appearing on console during the translation
    os.popen(cmd).readlines()

    assert os.path.exists(sas_file)

    # construct the FDR problem by reading from sas_file
    self.actions = set()
    self.goal = {}

    var_domain = {}
    fact_to_varval = {}
    varval_to_fact = {}

    lines = list(line.replace("\n", "") for line in open(sas_file, "r").readlines())

    self._i=0

    def getline():
      line = lines[self._i]
      self._i += 1
      return line
    
    while self._i < len(lines):
      line = getline()
      if line == "begin_variable":
        getline()  # var name
        var = len(var_domain)
        assert getline() == "-1"
        n_val = int(getline())
        var_domain[var] = n_val
        for val in range(n_val):
          line = getline()
          if "<none of those>" == line:
            fact = line
          else:
            toks = line.split()
            atomtype = toks[0]
            fact = "".join(toks[1:])
            pred = fact[:fact.index("(")]
            fact = fact.replace(pred+"(","").replace(")","")
            args = fact.split(",")
            if len(args) > 0:
              lime = f"({pred}"
              for j, arg in enumerate(args):
                lime+=f" {arg}"
                if j == len(args)-1:
                  lime+=")"
            else:
              lime = f"({pred})"
            fact = lime
            if atomtype == "NegatedAtom":
              fact = f"not {fact}"
            elif atomtype == "Atom":
              fact = fact
            else:
              raise ValueError
          fact_to_varval[fact] = (str(var), str(val))
          varval_to_fact[(str(var), str(val))] = fact
        assert "end_variable" in getline()
      elif line == "begin_state":  # no need to do anything
        pass
      elif line == "begin_operator":
        action_name = getline()
        preconditions = []
        add_effects = []
        del_effects = []

        n_prevail = int(getline())
        for _ in range(n_prevail):
          toks = getline().split()
          assert len(toks) == 2
          preconditions.append((toks[0],toks[1]))
        
        n_effects = int(getline())
        for _ in range(n_effects):
          toks = getline().split()
          assert toks[0] == "0" and len(toks) == 4  # no cond effects
          if toks[2] != "-1":
              preconditions.append((toks[1], toks[2]))
              del_effects.append((toks[1], toks[2]))
          add_effects.append((toks[1], toks[3]))

        assert getline() == "1"  # unit cost

        self.actions.add(Operator(action_name, preconditions, add_effects, del_effects))
        assert "end_operator" in getline()
      elif line == "begin_goal":
        n_goal = int(getline())
        for _ in range(n_goal):
          toks = getline().split()
          var = toks[0]
          val = toks[1]
          self.goal[var] = val
        assert "end_goal" in getline()

    self.fact_to_varval = fact_to_varval
    self.varval_to_fact = varval_to_fact

    os.remove(sas_file)
    return
    

def get_planning_problem(
    domain_pddl: str,
    problem_pddl: str,
    fdr: bool=False,
):
    # TODO: SLG can be optimised by also translating to FDR
    # TODO: can further optimise both FLG and SLG with preprocess-h2

    if fdr:
      problem = FDRProblem(domain_pddl=domain_pddl, problem_pddl=problem_pddl)
    else:
      problem: Task = planning.translate.pddl_parser.open(domain_filename=domain_pddl, task_filename=problem_pddl)

    return problem
