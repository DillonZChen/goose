import datetime
import os
import subprocess
from argparse import Namespace
from typing import Dict, List, Optional, Set, Tuple

import pymimir

from learner.dataset.state_data import StateData
from learner.problem.numeric_condition import NumericCondition
from learner.problem.numeric_domain import NumericDomain
from learner.problem.numeric_state import NumericState, str_to_state

## plan trace and succs, as well as list of actions
PlanTraceAndSuccs = Tuple[List[StateData], List[str]]


class NumericProblem:
    def __init__(
        self,
        domain_pddl: str,
        problem_pddl: str,
        opts: Namespace,
        nfd_vars_string: Optional[str] = None,
    ) -> None:
        self.domain_pddl: str = domain_pddl
        self.problem_pddl: str = problem_pddl
        self.numeric: bool = opts.numeric
        self._opts = opts

        ## lifted domain information
        self.domain = NumericDomain(domain_pddl)
        self.symbols: List[str] = self.domain.symbols
        self.schemata: List[str] = set(str(a.name) for a in self.domain.schemata)
        self.schema_to_index: Dict[str, int] = {
            s: i for i, s in enumerate(sorted(self.schemata))
        }
        self.object_types: List[str] = self.domain.object_types

        ## problem information
        self.initial_state: NumericState = None
        self.objects: List[str] = []
        self.num_goals: List[NumericCondition] = []
        self.bool_goals: List[str] = []

        ## these are only used when ~lifted ^ numeric
        ## contains all ground fluents and facts
        ## statics may or may not be contained based on opts
        self.fluents: List[str] = []
        self.facts: List[str] = []

        ## this can probably be done better with abstractions
        if self.numeric:
            self._init_nfd(nfd_vars_string)
        else:
            self._init_mmr()

    def _init_nfd(self, nfd_vars_string: str) -> None:
        ## parse pddl
        with open(self.problem_pddl, "r") as f:
            lines = f.readlines()
        output = ""
        for line in lines:
            ## TODO deal with inline comments
            if line.strip().startswith(";"):
                continue
            ## NFD parser makes everything lower case
            output += line.lower() + "\n"

        ## collect all nonstatic ground facts and fluents from NFD
        if nfd_vars_string is not None:
            facts, fluents = _get_nfd_vars_from_string(nfd_vars_string)
        else:
            facts, fluents = _get_nfd_vars(self.domain_pddl, self.problem_pddl)
        self.facts = facts
        self.fluents = fluents

        ## collect static facts and fluents by reading from the pddl file
        static_facts, static_fluents = _collect_statics(output, set(facts + fluents))
        if self._opts.static_facts:
            self.facts = sorted(list(set(self.facts).union(set(static_facts))))
        if self._opts.static_fluents:
            self.fluents = sorted(list(set(self.fluents).union(set(static_fluents))))
        _nfd_vars = set(self.facts + self.fluents)

        ## TODO read objects from pddl file instead
        objects = set()
        for var in _nfd_vars:
            var = var.replace(" ", "")
            var = var[var.find("(") + 1 : var.find(")")]
            for obj in var.split(","):
                if len(obj) > 0:
                    objects.add(obj)
        self.objects: List[str] = sorted(list(objects))

        ## parse initial state
        init_state = _parse_init_state(output)
        self.initial_state = str_to_state(init_state, _nfd_vars)

        ## parse goal condition
        goal = _parse_goal_condition(output)
        bool_goals, num_goals = str_to_state(goal, _nfd_vars, parse_goal=True)
        # print(bool_goals)
        # print(num_goals)
        # print(goal)
        self.bool_goals = bool_goals
        self.num_goals = [
            NumericCondition(expr, val, comparator, _nfd_vars)
            for expr, val, comparator in num_goals
        ]

    def _init_mmr(self) -> None:
        domain = pymimir.DomainParser(str(self.domain_pddl)).parse()
        problem = pymimir.ProblemParser(str(self.problem_pddl)).parse(domain)

        true_facts = [fact.get_name() for fact in problem.initial]
        self.initial_state = NumericState(true_facts, {})

        for obj in problem.objects:
            self.objects.append(obj.name)

        for goal in problem.goal:
            if goal.negated:
                print("WARNING: Negative goals are not supported.")
            self.bool_goals.append(goal.atom.get_name())

    def is_num_var(self, var: str) -> bool:
        if not self.numeric:
            return False
        assert isinstance(var, str)
        in_fluents = var in set(self.fluents)
        in_facts = var in set(self.facts)
        assert in_fluents or in_facts
        return in_fluents

    def trace_and_succs_from_plan_file(self, plan_file: str) -> PlanTraceAndSuccs:
        if not self.numeric:
            return self._parse_plan_mmr(plan_file)
        else:
            return self._parse_plan_nfd(plan_file)

    def _parse_plan_nfd(self, plan_file: str) -> PlanTraceAndSuccs:
        config = f"plan_trace_successors(plan_path={os.path.abspath(plan_file)})"
        output = _call_nfd(self.domain_pddl, self.problem_pddl, config)

        output = output[output.find("__START_HERE__") + len("__START_HERE__") :]
        output = output[: output.find("__END_HERE__")]
        lines = output.split("\n")

        actions = []
        states = []
        opt_states = set()

        def state_from_str(input: str) -> NumericState:
            if len(input) == 0:
                return None
            toks = input.split(";")
            facts = sorted(toks[0].split("?"))
            facts = set([f for f in facts if len(f) > 0])
            fluents = []
            for tok in toks[1].split("?"):
                if len(tok) == 0:
                    continue
                toks = tok.split(":")
                var = toks[0]
                val = float(toks[1])
                fluents.append((var, val))
            true_facts = []
            fluent_values = {}
            for fact in self.facts:
                if fact in facts:
                    true_facts.append(fact)
            for var, val in fluents:
                fluent_values[var] = val
            state = NumericState(true_facts, fluent_values)
            return state

        for line in lines:
            if line.startswith("_action^"):
                toks = line.split("^")
                action = toks[1].replace(" ", "_")
                actions.append(action)
            if line.startswith("_state^"):
                toks = line.split("^")
                numeric_state = state_from_str(toks[1])
                description = toks[2]
                assert description in {"opt", "unsolved", "dead"}
                if description == "opt":
                    opt_states.add(numeric_state)

        for line in lines:
            if line.startswith("_state^"):
                toks = line.split("^")
                # print(toks)
                numeric_state = state_from_str(toks[1])
                description = toks[2]
                assert description in {"opt", "unsolved", "dead"}
                if description == "unsolved" and numeric_state in opt_states:
                    continue
                heuristic = float(toks[3])
                parent_state = state_from_str(toks[4])
                if parent_state == numeric_state:
                    parent_state = None
                optimal_actions = [toks[5]] if len(toks) > 5 else None
                # print(nfd_state, optimal_actions)
                state = StateData(
                    state=numeric_state,
                    description=description,
                    heuristic=heuristic,
                    parent_state=parent_state,
                    optimal_actions=optimal_actions,
                )
                states.append(state)
        return states, actions

    def _parse_plan_mmr(self, plan_file: str) -> PlanTraceAndSuccs:
        domain = pymimir.DomainParser(str(self.domain_pddl)).parse()
        problem = pymimir.ProblemParser(str(self.problem_pddl)).parse(domain)
        succ_generator = pymimir.LiftedSuccessorGenerator(problem)

        name_to_schema: dict[str, pymimir.ActionSchema] = {
            schema.name: schema for schema in domain.action_schemas
        }

        name_to_object: dict[str, pymimir.Object] = {
            obj.name: obj for obj in problem.objects
        }

        actions: List[pymimir.Action] = []
        states: List[StateData] = []
        opt_states: Set[NumericState] = set()

        with open(plan_file, "r") as f:
            lines = f.readlines()
            for line in lines:
                if line.startswith(";"):
                    continue
                action_name = line.strip()
                action_name = action_name.replace("(", "")
                action_name = action_name.replace(")", "")
                toks = action_name.split(" ")
                schema = toks[0]
                schema = name_to_schema[schema]
                args = toks[1:]
                args = [name_to_object[arg] for arg in args]
                action = pymimir.Action.new(problem, schema, args)
                actions.append(action)

        def mimir_to_my_state(state: pymimir.State) -> NumericState:
            true_facts = []
            for fact in state.get_atoms():
                if fact.predicate.name in self.domain.static_symbols:
                    continue
                true_facts.append(fact.get_name())
            fluent_values = {}  ## TODO fluents from mimir
            return NumericState(true_facts, fluent_values)

        state = problem.create_state(problem.initial)
        my_state = mimir_to_my_state(state)
        opt_actions = [actions[0].get_name()] if len(actions) > 0 else []
        state_data = StateData(
            state=my_state,
            parent_state=None,
            description="opt",
            heuristic=len(actions),
            optimal_actions=opt_actions,
        )
        states.append(state_data)
        for i, action in enumerate(actions):
            next_state = None
            succ_actions = succ_generator.get_applicable_actions(state)
            my_state = mimir_to_my_state(state)
            for a2 in succ_actions:
                succ_state = a2.apply(state)
                my_succ_state = mimir_to_my_state(succ_state)
                if action.get_name() == a2.get_name():
                    next_state = succ_state
                    opt_states.add(my_state)
                    desc = "opt"
                    h = len(actions) - i - 1
                elif succ_state in opt_states:
                    continue
                else:
                    desc = "unsolved"
                    h = 2147483647
                state_data = StateData(
                    state=my_succ_state,
                    parent_state=my_state,
                    description=desc,
                    heuristic=h,
                    optimal_actions=[action.get_name()],
                )
                states.append(state_data)
            assert next_state is not None, "succ_generator did find opt action"
            state = next_state

        return states, actions

    def dump(self) -> None:
        print(f"domain_pddl: {self.domain_pddl}")
        print(f"problem_pddl: {self.problem_pddl}")
        print(f"numeric: {self.numeric}")
        print(f"max_var_arity: {self.domain.max_func_arity}")
        print(f"max_bool_var_arity: {self.domain.max_pred_arity}")
        print(f"max_action_arity: {self.domain.max_action_arity}")
        print(f"n_fluents: {len(self.fluents)}")
        print(f"n_facts: {len(self.facts)}")
        print(f"n_num_goals: {len(self.num_goals)}")
        print(f"n_bool_goals: {len(self.bool_goals)}")
        print(f"n_schemata: {len(self.schemata)}")


def _get_nfd_vars_from_string(input: str) -> Tuple[List[str], List[str]]:
    state = input[input.find("__START_HERE__") + len("__START_HERE__") :]
    state = state.split("\n")[0]

    facts = state.split(";")[0].split("?")
    facts = [f for f in facts if f != ""]
    facts = sorted(list(set(facts)))

    fluents = state.split(";")[1].split("?")
    fluents = [f for f in fluents if f != ""]
    fluents = sorted(list(set(fluents)))

    return (facts, fluents)


def _get_nfd_vars(domain_pddl: str, problem_pddl: str) -> Tuple[List[str], List[str]]:
    output = _call_nfd(domain_pddl, problem_pddl, "perfect()")

    if "dummy" in output and "," not in output:
        print(output)
        print("The predicate 'dummy' is detected in the problem variables.")
        print("NFD detected that the problem is trivial or unsolvable.")
        print("Terminating...")
        exit(-1)

    return _get_nfd_vars_from_string(output)


def _call_nfd(domain_pddl: str, problem_pddl: str, config: str) -> str:
    ## check if built yet
    nfd_path = "planner/nfd"
    if "NGOOSE" in os.environ:
        nfd_path = os.environ["NGOOSE"] + "/" + nfd_path
    build_path = f"{nfd_path}/builds"
    if not os.path.exists(build_path):
        print("Error: NFD not built yet. You can build it with `sh setup.sh`.")
        print("Terminating...")
        exit(-1)

    ## try to generate a unique temp file name in case trainers are parallelised
    dt = datetime.datetime.now().strftime("%I:%M%p on %B %d, %Y")
    process = subprocess.Popen(["lscpu"], stdout=subprocess.PIPE)
    output, error = process.communicate()
    output = output.decode()
    sas_file = domain_pddl + problem_pddl + dt + repr(hash(output))
    for tok in ["/", ".", "-", " ", "\n"]:
        sas_file = sas_file.replace(tok, "_")
    sas_file += ".sas"

    ## call NFD
    cmd = [
        "python2",
        "fast-downward.py",
        "--build",
        "release64",
        "--sas_file",
        sas_file,
        os.path.abspath(domain_pddl),
        os.path.abspath(problem_pddl),
        "--search",
        config,
    ]
    pipe = subprocess.PIPE
    process = subprocess.Popen(cmd, stdout=pipe, stderr=pipe, cwd=nfd_path)
    output, error = process.communicate()
    output = output.decode()
    error = error.decode()

    # print(output, flush=True)
    # print(error, flush=True)

    return output


def _parse_init_state(output: str) -> str:
    init_state = output[output.find("(:init") + len("(:init") :]
    stack = 0
    for i, char in enumerate(init_state):
        if char == "(":
            stack += 1
        elif char == ")":
            stack -= 1
            if stack < 0:
                init_state = init_state[:i]
                break
    return init_state


def _parse_goal_condition(output: str) -> str:
    goal = output[output.find("(:goal") + len("(:goal") :]
    goal = goal.strip()
    if not goal.startswith("(and") or "(not " in goal:
        print("Expected goal condition of the form")
        print("(:goal (and (b1) (b2) ... (= (n1) v1) (= (n2) v2) ... ))")
        print("- Fact and fluent order does not matter, but expected '(and'")
        print("- Negative goals are also *not* supported")
        print("Got goal condition:")
        print(goal)
        print("Exiting...", flush=True)
        exit(-1)
    goal = goal[len("(and") :]
    stack = 0
    for i, char in enumerate(goal):
        if char == "(":
            stack += 1
        elif char == ")":
            stack -= 1
            if stack < 0:
                goal = goal[:i]
    return goal


def _collect_statics(nfd_output, nfd_vars):
    str_state = _parse_init_state(nfd_output)
    return str_to_state(str_state, nfd_vars, collect_static=True)
