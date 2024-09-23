import os
from typing import Optional

import pymimir
from _wlplan.planning import Atom, Domain, Predicate, Problem

State = list[Atom]


def _get_predicates(mimir_domain: pymimir.Domain, keep_statics: bool) -> dict[str, Predicate]:
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


def parse_domain(domain_path: str, domain_name: str = None, keep_statics: bool = True) -> Domain:
    """Parses a domain file and returns a Domain object.

    Args:
        domain_path (str): The path to the domain file.
        domain_name (str, optional): The name of the domain. If not provided, it will be extracted from the file. Defaults to None.
        keep_statics (bool, optional): Whether to keep static predicates in the domain, computed by taking the union of action effects. Defaults to True.
    """

    if not os.path.exists(domain_path):
        raise FileNotFoundError(f"Domain file {domain_path} does not exist.")

    with open(domain_path, "r") as f:
        domain_content = f.read()

    # Read domain name from file if not provided
    if domain_name is None:
        domain_name = domain_content.split("(domain ")[1]
        domain_name = domain_name.split(")")[0]

    # Parse domain with mimir
    mimir_domain = pymimir.DomainParser(str(domain_path)).parse()

    # Get predicates
    predicates = _get_predicates(mimir_domain, keep_statics)
    predicates = sorted(list(predicates.values()), key=lambda x: repr(x))

    # Get constant objects (ignores types)
    constant_objects = set()
    if "(:constants" in domain_content:
        constant_objects_content = domain_content.split("(:constants")[1]
        constant_objects_content = constant_objects_content.split(")")[0]
        for line in constant_objects_content.split("\n"):
            line = line.strip()
            if len(line) == 0:
                continue
            lhs_rhs = line.split("-")  # type is on the rhs
            objects = lhs_rhs[0].split()
            constant_objects = constant_objects.union(set(objects))
    constant_objects = sorted(list(constant_objects))

    domain = Domain(name=domain_name, predicates=predicates, constant_objects=constant_objects)
    return domain


def parse_problem(domain_path: str, problem_path: str, keep_statics: bool = True) -> Problem:
    """Parses a problem file and returns a Problem object.

    Args:
        domain_path (str): The path to the domain file.
        problem_path (str): The path to the problem file.
        keep_statics (bool, optional): Whether to keep static predicates in the parsed domain. Defaults to True.
    """

    if not os.path.exists(problem_path):
        raise FileNotFoundError(f"Problem file {problem_path} does not exist.")

    # A lot of redundancy occurs in this method, but not a problem since parsing should be fast.

    # Get wlplan domain
    domain = parse_domain(domain_path)

    # Use mimir to help with parsing
    mimir_domain = pymimir.DomainParser(str(domain_path)).parse()
    mimir_problem = pymimir.ProblemParser(str(problem_path)).parse(mimir_domain)

    name_to_predicate = _get_predicates(mimir_domain, keep_statics)
    name_to_object = {o.name: o for o in mimir_problem.objects}
    objects = sorted(list(name_to_object.keys()))

    ## Get goal information
    positive_goals = []
    negative_goals = []
    for literal in mimir_problem.goal:
        mimir_atom = literal.atom
        wlplan_atom = Atom(
            predicate=name_to_predicate[mimir_atom.predicate.name],
            objects=[o.name for o in mimir_atom.terms],
        )
        if literal.negated:
            negative_goals.append(wlplan_atom)
        else:
            positive_goals.append(wlplan_atom)

    problem = Problem(
        domain=domain,
        objects=objects,
        positive_goals=positive_goals,
        negative_goals=negative_goals,
    )
    return problem
