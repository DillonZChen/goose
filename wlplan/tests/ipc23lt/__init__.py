import logging
import os
import zipfile

import pymimir

import wlplan
from wlplan.data import Dataset, ProblemStates
from wlplan.planning import Predicate, parse_domain

LOGGER = logging.getLogger(__name__)
DOMAINS = {
    "blocksworld",
    "childsnack",
    "ferry",
    "floortile",
    "miconic",
    "rovers",
    "satellite",
    "sokoban",
    "spanner",
    "transport",
}


def get_mimir_problem(domain_pddl: str, problem_pddl: str):
    mimir_domain = pymimir.DomainParser(str(domain_pddl)).parse()
    mimir_problem = pymimir.ProblemParser(str(problem_pddl)).parse(mimir_domain)
    return mimir_domain, mimir_problem


def get_domain_benchmark_dir(domain_name: str):
    assert domain_name in DOMAINS

    file_dir = os.path.dirname(os.path.abspath(__file__))

    ret = f"{file_dir}/benchmarks/{domain_name}"
    if not os.path.exists(ret):
        zip_file_path = f"{file_dir}/benchmarks.zip"
        extract_dir = f"{file_dir}"

        with zipfile.ZipFile(zip_file_path, "r") as zip_ref:
            zip_ref.extractall(extract_dir)

    return ret


def get_domain_pddl(domain_name: str):
    assert domain_name in DOMAINS
    benchmark_dir = get_domain_benchmark_dir(domain_name)
    domain_pddl = f"{benchmark_dir}/domain.pddl"
    return domain_pddl


def get_problem_pddl(domain_name: str, problem_name: str):
    assert domain_name in DOMAINS
    benchmark_dir = get_domain_benchmark_dir(domain_name)
    problem_pddl = f"{benchmark_dir}/testing/p{problem_name}.pddl"
    assert os.path.exists(problem_pddl), problem_pddl
    return problem_pddl


def get_predicates(mimir_domain: pymimir.Domain, keep_statics: bool):
    predicates = {}
    if keep_statics:
        for predicate in mimir_domain.predicates:
            name = predicate.name
            arity = predicate.arity
            predicates[name] = Predicate(name=name, arity=arity)
    else:
        for schema in mimir_domain.action_schemas:
            for effect in schema.effect:
                atom = effect.atom
                predicate = atom.predicate
                name = predicate.name
                arity = predicate.arity
                predicate = Predicate(name=name, arity=arity)
                if name not in predicates:
                    predicates[name] = predicate
                else:
                    assert predicates[name] == predicate
    return predicates


def get_raw_dataset(domain_name: str, keep_statics: bool):
    assert domain_name in DOMAINS

    benchmark_dir = get_domain_benchmark_dir(domain_name)

    domain_pddl = f"{benchmark_dir}/domain.pddl"
    mimir_domain = pymimir.DomainParser(str(domain_pddl)).parse()

    name_to_predicate = get_predicates(mimir_domain, keep_statics)
    predicates = sorted(list(name_to_predicate.values()), key=lambda x: repr(x))
    predicate_names = repr([repr(x) for x in predicates]).replace("'", "")
    LOGGER.info(f"{domain_name} predicates for {keep_statics=}: {predicate_names}")

    wlplan_domain = parse_domain(domain_pddl, domain_name, keep_statics)

    wlplan_data = []
    y = []

    for f in os.listdir(f"{benchmark_dir}/training_plans"):
        problem_pddl = f"{benchmark_dir}/training/" + f.replace(".plan", ".pddl")
        plan_file = f"{benchmark_dir}/training_plans/" + f

        ## parse problem with mimir
        mimir_problem = pymimir.ProblemParser(str(problem_pddl)).parse(mimir_domain)
        mimir_state = mimir_problem.create_state(mimir_problem.initial)

        name_to_schema = {s.name: s for s in mimir_domain.action_schemas}
        name_to_object = {o.name: o for o in mimir_problem.objects}

        ## construct wlplan problem
        positive_goals = []
        for literal in mimir_problem.goal:
            assert not literal.negated
            mimir_atom = literal.atom
            wlplan_atom = wlplan.planning.Atom(
                predicate=name_to_predicate[mimir_atom.predicate.name],
                objects=[o.name for o in mimir_atom.terms],
            )
            positive_goals.append(wlplan_atom)

        wlplan_problem = wlplan.planning.Problem(
            domain=wlplan_domain,
            objects=list(name_to_object.keys()),
            positive_goals=positive_goals,
            negative_goals=[],
        )

        ## collect actions
        actions = []
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
                action = pymimir.Action.new(mimir_problem, schema, args)
                actions.append(action)

        ## collect plan trace states
        wlplan_states = []

        def mimir_to_wlplan_state(mimir_state: pymimir.State):
            wlplan_state = []
            for atom in mimir_state.get_atoms():
                predicate_name = atom.predicate.name
                if predicate_name not in name_to_predicate:
                    continue
                wlplan_atom = wlplan.planning.Atom(
                    predicate=name_to_predicate[predicate_name],
                    objects=[o.name for o in atom.terms],
                )
                wlplan_state.append(wlplan_atom)
            return wlplan_state

        h_opt = len(actions)
        wlplan_states.append(mimir_to_wlplan_state(mimir_state))
        y.append(h_opt)
        for action in actions:
            h_opt -= 1
            mimir_state = action.apply(mimir_state)
            wlplan_states.append(mimir_to_wlplan_state(mimir_state))
            y.append(h_opt)

        wlplan_data.append((wlplan_problem, wlplan_states))

    return wlplan_domain, wlplan_data, y


def get_dataset(domain_name: str, keep_statics: bool):
    domain, data, y = get_raw_dataset(domain_name, keep_statics)
    problem_states = []
    for problem, states in data:
        problem_states.append(ProblemStates(problem=problem, states=states))
    dataset = Dataset(domain=domain, data=problem_states)
    return domain, dataset, y
