import argparse
import os
import random

from tqdm import tqdm
from unified_planning.io import PDDLWriter
from unified_planning.io.pddl_reader import PDDLReader
from unified_planning.shortcuts import *

"""
Ferry can now board up to 4 cars at a time.
"""


def var_to_objects(var: str) -> List[str]:
    var = str(var)
    objects = var.split("(")
    if len(objects) == 1:
        return []
    objects = objects[1].replace(")", "").split(", ")
    if len(objects) == 1 and objects[0] == "":
        return []
    return objects


def var_to_predicate(var: str) -> str:
    var = str(var)
    return var.split("(")[0]


def get_initial_facts_and_goals(problem):
    facts = []
    goals = []
    for fact, val in problem._initial_value.items():
        assert str(val) == "true"
        facts.append(fact)

    goals: Iterable[FNode] = problem.goals
    assert len(goals) == 1
    while isinstance(goals, list):
        assert len(goals) == 1
        goals = goals[0]
    if goals.is_and():
        goals = goals.args
    else:
        # singleton goal e.g. ipc2023-learning ferry
        assert goals.is_fluent_exp() or goals.is_variable_exp(), goals
        goals = [goals]

    return (facts, goals)


def main():

    domain_name = f"{os.path.basename(os.getcwd())}_numeric"

    seed = 0
    random.seed(seed)
    classic_domain = "classic/domain.pddl"
    classic_train_dir = "classic/training"
    numeric_train_dir = "numeric/training"
    classic_test_dir = "classic/testing_flattened"
    numeric_test_dir = "numeric/testing"
    os.system(f"rm -rf numeric/testing_flattened")
    os.system(f"rm -rf {numeric_train_dir}")
    os.system(f"rm -rf {numeric_test_dir}")
    os.makedirs(numeric_train_dir, exist_ok=True)
    os.makedirs(numeric_test_dir, exist_ok=True)

    for c_dir, n_dir in [
        (classic_train_dir, numeric_train_dir),
        (classic_test_dir, numeric_test_dir),
    ]:
        for f_name in tqdm(sorted(os.listdir(c_dir))):
            if ".pddl" not in f_name:
                continue
            classic_pddl = f"{c_dir}/{f_name}"
            reader = PDDLReader()
            problem = reader.parse_problem("numeric/domain.pddl", classic_pddl)
            facts, goals = get_initial_facts_and_goals(problem)
            user_types = {obj.name: obj for obj in problem.user_types}
            fluents = {fluent.name: fluent for fluent in problem.fluents}

            """""" """""" """""" """""" """""" """""" """""" """""" """""" """"""
            """""" """""" """ actual problem writing starts here """ """""" """"""
            """""" """""" """""" """""" """""" """""" """""" """""" """""" """"""

            ### initial state
            problem.set_initial_value(fluents["ferry-capacity"], 0)
            problem.set_initial_value(fluents["empty-ferry"], False)

            ### goal state
            # problem.add_goal(Equals(fluents["ferry-capacity"], 0))

            ### I/O
            writer = PDDLWriter(problem)
            problem_pddl = f"{n_dir}/{f_name}"
            writer.write_problem(problem_pddl)
            output = open(problem_pddl, "r").read()
            output = output.replace("at_ ", "at ")
            output = output.replace("at_ferry", "at-ferry")
            output = output.replace("ferry_capacity", "ferry-capacity")
            with open(problem_pddl, "w") as f:
                f.write(output)


if __name__ == "__main__":
    main()
