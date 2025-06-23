import argparse
import os

import pymimir

CUR_DIR = os.path.dirname(os.path.abspath(__file__))
ROOT_DIR = f"{CUR_DIR}/.."
VALIDATOR = os.path.abspath(f"{ROOT_DIR}/planning/numeric-downward/validate_plan.py")


def validate_from_log(domain_pddl: str, problem_pddl: str, log_file: str):
    assert os.path.exists(domain_pddl), domain_pddl
    assert os.path.exists(problem_pddl), problem_pddl
    assert os.path.exists(log_file), log_file

    plan = []
    reading_plan = False
    with open(log_file, "r") as f:
        content = f.read()
        if "Solution found!" not in content:
            print("No solution found from log")
            return
        for line in content[content.find("Solution found!") :].split("\n")[1:]:
            if not line.startswith("["):
                plan.append(line)
                reading_plan = True
            elif reading_plan:
                break

    plan_content = ""
    for action in plan:
        plan_content += "(" + action.replace("(1)", "").strip() + ")" + "\n"

    plan_file = "plan_to_validate.plan"
    with open(plan_file, "w") as f:
        f.write(plan_content)

    validate(domain_pddl, problem_pddl, plan_file)


def atom_to_str(atom: pymimir.Atom):
    return f"{atom.predicate.name}({', '.join([o.name for o in atom.terms])})"


def validate(domain_pddl: str, problem_pddl: str, plan_file: str):
    assert os.path.exists(domain_pddl), domain_pddl
    assert os.path.exists(problem_pddl), problem_pddl
    assert os.path.exists(plan_file), plan_file

    domain = pymimir.DomainParser(domain_pddl).parse()
    problem = pymimir.ProblemParser(problem_pddl).parse(domain)

    name_to_action_schema = {action.name: action for action in domain.action_schemas}
    name_to_objects = {o.name: o for o in problem.objects}

    plan = []
    with open(plan_file, "r") as f:
        plan_content = f.read()
        for line in plan_content.split("\n"):
            if line.strip() == "" or line.startswith(";"):
                continue
            plan.append(line.replace("(", "").replace(")", "").strip())

    state = problem.create_state(problem.initial)
    for action in plan:
        toks = action.split()
        schema = toks[0]
        if schema not in name_to_action_schema:
            print(f"Bad: Action schema in action ({action}) not found")
            return
        objects = toks[1:]
        if not all(o in name_to_objects for o in objects):
            print(f"Bad: Objects in action ({action}) not found")
            return
        action_schema = name_to_action_schema[schema]
        action = pymimir.Action.new(problem, action_schema, [name_to_objects[tok] for tok in objects])
        state = action.apply(state)

    goal = problem.goal
    state = set([atom_to_str(f) for f in state.get_atoms()])
    pos_goals = set([atom_to_str(f.atom) for f in goal if not f.negated])
    neg_goals = set([atom_to_str(f.atom) for f in goal if f.negated])
    if not pos_goals.issubset(state):
        print("Bad: Positive goals not achieved")
        return
    if not neg_goals.isdisjoint(state):
        print("Bad: Negative goals achieved")
        return

    print("GOOD!")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("domain_pddl")
    parser.add_argument("problem_pddl")
    parser.add_argument("plan_or_log_file")
    args = parser.parse_args()
    if args.plan_or_log_file.endswith(".log"):
        validate_from_log(args.domain_pddl, args.problem_pddl, args.plan_or_log_file)
    else:
        validate(args.domain_pddl, args.problem_pddl, args.plan_or_log_file)
