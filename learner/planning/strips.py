import os
import pickle
import sys
import pddl_parser
from abc import ABC, abstractmethod
from datetime import datetime
from typing import FrozenSet, List, NamedTuple, TypeVar
from tqdm.auto import tqdm
from dataset import get_domain_name, get_problem_name

# Proposition - i.e. fact, atom
Proposition = TypeVar("Proposition", bound=str)
Number = TypeVar("Number", int, float)
State = FrozenSet[Proposition]

SEP = ":"

# STRIPSAction which essentially represents a hyperedge
class STRIPSAction(NamedTuple):
    name: str
    cost: Number
    preconditions: FrozenSet[Proposition]
    add_effects: FrozenSet[Proposition]
    del_effects: FrozenSet[Proposition]

    def applicable(self, state):
        """
        Operators are applicable when their set of preconditions is a subset
        of the facts that are true in "state".

        @return True if the operator's preconditions is a subset of the state,
                False otherwise
        """
        return self.preconditions <= state

    def apply(self, state: FrozenSet[Proposition]) -> FrozenSet[Proposition]:
        """
        Apply an action in a given state to get the new state.

        Parameters
        ----------
        state: FrozenSet[Proposition], current state

        Returns
        -------
        FrozenSet[Proposition], new state after applying this action
        """
        # Check we can actually apply this action
        if not self.preconditions.issubset(state):
            raise RuntimeError(f"Cannot apply {self.name} in state {state}")

        # Compute new state: (s \ Del(o)) U Add(o)
        return (state.difference(self.del_effects)).union(self.add_effects)


class STRIPSProblem(ABC):
    """ Abstract STRIPS Problem """

    def __init__(self, domain_pddl: str, problem_pddl: str):
        self.domain_pddl = domain_pddl
        self.problem_pddl = problem_pddl

    @property
    @abstractmethod
    def domain_name(self) -> str:
        raise NotImplementedError

    @property
    @abstractmethod
    def name(self) -> str:
        raise NotImplementedError

    @property
    @abstractmethod
    def initial_state(self) -> FrozenSet[Proposition]:
        raise NotImplementedError

    @property
    @abstractmethod
    def goals(self) -> FrozenSet[Proposition]:
        raise NotImplementedError

    def is_goal_state(self, state: FrozenSet[Proposition]) -> bool:
        """ Whether the given state is a goal state """
        return self.goals.issubset(state)

    @property
    @abstractmethod
    def propositions(self) -> List[Proposition]:
        raise NotImplementedError

    @property
    def number_of_propositions(self) -> int:
        return len(self.propositions)

    @property
    def facts(self) -> List[Proposition]:
        return self.propositions

    @property
    @abstractmethod
    def actions(self) -> List[STRIPSAction]:
        raise NotImplementedError

    @property
    def number_of_actions(self) -> int:
        return len(self.actions)

    @property
    def operators(self) -> List[STRIPSAction]:
        return self.actions

    def goal_reached(self, state):
        """
        The goal has been reached if all facts that are true in "goals"
        are true in "state".

        @return True if all the goals are reached, False otherwise
        """
        return self.goals <= state

    def get_successor_states(self, state):
        """
        @return A list with (op, new_state) pairs where "op" is the applicable
        operator and "new_state" the state that results when "op" is applied
        in state "state".
        """
        return [(op, op.apply(state)) for op in self.actions if op.applicable(state)]


class FDRProblem():
    def __init__(self, domain_pddl: str, problem_pddl: str):

        dt = datetime.now().strftime("%d_%m_%Y_%H_%M_%S")
        sas_file = f"LIMEoutput_{dt}_{os.path.basename(problem_pddl)}_{domain_pddl.replace('/', '')}.sas"
        if os.path.exists("downward/fast-downward.py"):
          cmd = f"./downward/fast-downward.py --sas-file {sas_file} {domain_pddl} {problem_pddl}"
          os.popen(cmd).readlines()
        else:  # hack for running from cpp
          script= os.path.expandvars(f"${{PLAN_GNN}}/src/downward/fast-downward.py")
          assert os.path.exists(script)
          cmd = f"python3 {script} --sas-file {sas_file} {domain_pddl} {problem_pddl}"
          os.popen(cmd).readlines()
        assert os.path.exists(sas_file)

        self.name = os.path.basename(problem_pddl).replace(".pddl", "")
        self.actions = set()
        self.goals = {}

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
                    self.goals[var] = val
                assert "end_goal" in getline()

        self.fact_to_varval = fact_to_varval
        self.varval_to_fact = varval_to_fact

        os.remove(sas_file)
    

def get_strips_problem(
    domain_pddl: str,
    problem_pddl: str,
):
    """
    A factory-ish pattern to abstract the underlying implementation using
    Pyperplan away
    """

    """ saving causes us problems when using with pybind11 """
    problem = pddl_parser.open(domain_filename=domain_pddl, task_filename=problem_pddl)
    # if parser=="preprocess-h2":  # not actually using prperocess-h2 anymore 
    #     problem = FDRProblem(domain_pddl=domain_pddl, problem_pddl=problem_pddl)
    # elif parser=="downward":
    #     problem = pddl_parser.open(domain_filename=domain_pddl, task_filename=problem_pddl)

    return problem