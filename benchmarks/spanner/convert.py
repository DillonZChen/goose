import argparse
import os
import random

from tqdm import tqdm
from unified_planning.io import PDDLWriter
from unified_planning.io.pddl_reader import PDDLReader
from unified_planning.shortcuts import *

"""
Remove all symmetries with numeric
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

    def make_problem(p_name):
        reader = PDDLReader()
        problem = reader.parse_problem("numeric/domain.pddl")
        return problem

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
            c_problem = reader.parse_problem(classic_domain, classic_pddl)
            facts, goals = get_initial_facts_and_goals(c_problem)

            locs = {}  # map to number of spanners
            links = []
            nuts = 0

            for obj in c_problem.all_objects:
                obj_name = str(obj)
                if obj_name.startswith("location"):
                    locs[obj_name] = 0

            for fact in facts:
                pred = var_to_predicate(fact)
                objects = var_to_objects(fact)
                if pred == "at" and len(objects) == 2:
                    loc = objects[1]
                    if loc not in locs:
                        locs[loc] = 0
                    if not str(objects[0]).startswith("spanner"):
                        continue
                    locs[loc] += 1
                elif pred == "link":
                    loc_from = objects[0]
                    loc_to = objects[1]
                    links.append(fact)
                elif pred == "loose":
                    nuts += 1

            print(f"l={len(locs)}, n={nuts}, s={sum(locs.values())}")

            """""" """""" """""" """""" """""" """""" """""" """""" """""" """"""
            """""" """""" """ actual problem writing starts here """ """""" """"""
            """""" """""" """""" """""" """""" """""" """""" """""" """""" """"""

            n_problem = make_problem(f_name.replace(".pddl", ""))
            user_types = {obj.name: obj for obj in n_problem.user_types}
            fluents = {fluent.name: fluent for fluent in n_problem.fluents}

            bob = Object("bob", user_types["man"])
            loccc = [Object(f"location{i}", user_types["location"]) for i in range(1, len(locs) - 2 + 1)]
            shed = Object("shed", user_types["location"])
            gate = Object("gate", user_types["location"])
            loc_objects  = [shed]
            loc_objects += loccc
            # loc_objects += [gate]  # added as constant in domain.pddl
            n_problem.add_objects([bob] + loc_objects)

            for i in range(1, len(loc_objects)):
                link = fluents["link"](loc_objects[i - 1], loc_objects[i])
                n_problem.set_initial_value(link, True)

            for i in range(1, len(locs) - 2 + 1):
                loc_obj = loccc[i - 1]
                spanners = locs[f"location{i}"]
                n_problem.set_initial_value(fluents["spanners_at"](loc_obj), spanners)
            n_problem.set_initial_value(fluents["spanners_at"](shed), 0)
            n_problem.set_initial_value(fluents["spanners_at"](gate), 0)
            n_problem.set_initial_value(fluents["at"](bob, shed), True)
            n_problem.set_initial_value(fluents["carrying"](bob), 0)

            n_problem.set_initial_value(fluents["loose"], nuts)
            n_problem.add_goal(Equals(fluents["loose"], 0))

            ### I/O
            writer = PDDLWriter(n_problem)
            problem_pddl = f"{n_dir}/{f_name}"
            writer.write_problem(problem_pddl)
            output = open(problem_pddl, "r").read()
            output = output.replace("at_ ", "at ")
            output = output.replace("(at bob shed)", f"(at bob shed) (link {loccc[-1]} gate) ")
            with open(problem_pddl, "w") as f:
                f.write(output)


if __name__ == "__main__":
    main()
