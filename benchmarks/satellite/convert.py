import argparse
import os
import random
from itertools import combinations, product

import numpy as np
from tqdm import tqdm
from unified_planning.io import PDDLWriter
from unified_planning.io.pddl_reader import PDDLReader
from unified_planning.shortcuts import *

"""
Lift can now board up to 6 passengers at a time.
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
        sats = []
        dirs = []
        mms = []
        gs = []
        for f_name in tqdm(sorted(os.listdir(c_dir))):
            if ".pddl" not in f_name:
                continue
            classic_pddl = f"{c_dir}/{f_name}"
            reader = PDDLReader()
            problem = reader.parse_problem("numeric/domain.pddl", classic_pddl)
            facts, goals = get_initial_facts_and_goals(problem)
            user_types = {obj.name: obj for obj in problem.user_types}
            fluents = {fluent.name: fluent for fluent in problem.fluents}

            satellites = [obj for obj in problem.all_objects if str(obj.type) == "satellite"]
            directions = [obj for obj in problem.all_objects if str(obj.type) == "direction"]
            modes = [obj for obj in problem.all_objects if str(obj.type) == "mode"]

            n_goals = len(goals)

            sats.append(len(satellites))
            dirs.append(len(directions))
            mms.append(len(modes))
            gs.append(n_goals)

            """""" """""" """""" """""" """""" """""" """""" """""" """""" """"""
            """""" """""" """ actual problem writing starts here """ """""" """"""
            """""" """""" """""" """""" """""" """""" """""" """""" """""" """"""

            for satellite in satellites:
                problem.set_initial_value(fluents["data_capacity"](satellite), 7)
                problem.set_initial_value(fluents["fuel"](satellite), 7)

            for direction, mode in product(directions, modes):
                data = random.randint(1, 3)
                problem.set_initial_value(fluents["data"](direction, mode), data)

            for d1, d2 in product(directions, directions):
                slew_time = random.randint(1, 3)
                problem.set_initial_value(fluents["slew_time"](d1, d2), slew_time)

            ### I/O
            writer = PDDLWriter(problem)
            problem_pddl = f"{n_dir}/{f_name}"
            writer.write_problem(problem_pddl)
            output = open(problem_pddl, "r").read()
            output = output.replace("lift_at", "lift-at")
            with open(problem_pddl, "w") as f:
                f.write(output)

        print(f"Satellites {np.mean(sats):.2f} ± {np.std(sats):.2f}")
        print(f"Directions {np.mean(dirs):.2f} ± {np.std(dirs):.2f}")
        print(f"Modes {np.mean(mms):.2f} ± {np.std(mms):.2f}")
        print(f"Goals {np.mean(gs):.2f} ± {np.std(gs):.2f}")


if __name__ == "__main__":
    main()
