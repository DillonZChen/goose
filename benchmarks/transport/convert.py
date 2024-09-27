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

            capacities = {}  # vehicle to its max cap
            vehicle_at = {}
            package_at = {}
            roads = set()
            locations = set()

            for fact in facts:
                pred = var_to_predicate(fact)
                objects = var_to_objects(fact)
                if pred == "at" and len(objects) == 2:
                    obj = objects[0]
                    loc = objects[1]
                    if obj.startswith("v"):
                        vehicle_at[obj] = loc
                    else:
                        assert obj.startswith("p")
                        package_at[obj] = loc
                    locations.add(loc)
                elif pred == "road":
                    locations.add(objects[0])
                    locations.add(objects[1])
                    roads.add(fact)
                elif pred == "capacity":
                    vehicle = objects[0]
                    cap = int(objects[1].replace("c", ""))
                    capacities[vehicle] = cap

            assert set(capacities.keys()) == set(vehicle_at.keys())

            """""" """""" """""" """""" """""" """""" """""" """""" """""" """"""
            """""" """""" """ actual problem writing starts here """ """""" """"""
            """""" """""" """""" """""" """""" """""" """""" """""" """""" """"""

            n_problem = make_problem(f_name.replace(".pddl", ""))
            user_types = {obj.name: obj for obj in n_problem.user_types}
            fluents = {fluent.name: fluent for fluent in n_problem.fluents}

            str_to_location = {l: Object(l, user_types["location"]) for l in locations}
            str_to_package = {p: Object(p, user_types["package"]) for p in package_at.keys()}
            str_to_vehicle = {v: Object(v, user_types["vehicle"]) for v in vehicle_at.keys()}

            n_problem.add_objects(list(str_to_location.values()) + list(str_to_package.values()) + list(str_to_vehicle.values()))

            for v, loc in vehicle_at.items():
                n_problem.set_initial_value(fluents["capacity"](str_to_vehicle[v]), capacities[v])
            
            for p, loc in package_at.items():
                n_problem.set_initial_value(fluents["at"](str_to_package[p], str_to_location[loc]), True)

            for v, loc in vehicle_at.items():
                n_problem.set_initial_value(fluents["at"](str_to_vehicle[v], str_to_location[loc]), True)

            for road in roads:
                loc_from, loc_to = var_to_objects(road)
                n_problem.set_initial_value(fluents["road"](str_to_location[loc_from], str_to_location[loc_to]), True)
                
            for goal in goals:
                n_problem.add_goal(goal)

            ### I/O
            writer = PDDLWriter(n_problem)
            problem_pddl = f"{n_dir}/{f_name}"
            writer.write_problem(problem_pddl)
            output = open(problem_pddl, "r").read()
            output = output.replace("at_ ", "at ")
            output = output.replace("capacity_predecessor ", "capacity-predecessor ")
            with open(problem_pddl, "w") as f:
                f.write(output)


if __name__ == "__main__":
    main()
