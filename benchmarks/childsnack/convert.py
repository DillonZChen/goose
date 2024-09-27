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

    ### Types
    Tray = UserType("Tray")
    Place = UserType("Place")
    Gluten = UserType("Gluten")

    ### Variables
    # Boolean
    at = Fluent("at", BoolType(), tray=Tray, place=Place)
    is_gluten_free = Fluent("gluten_free", BoolType(), gluten=Gluten)

    # Numeric
    at_kitchen_bread = Fluent("at_kitchen_bread", RealType(), gluten=Gluten)
    at_kitchen_content = Fluent("at_kitchen_content", RealType(), gluten=Gluten)
    at_kitchen_sandwich = Fluent("at_kitchen_sandwich", RealType(), gluten=Gluten)
    ontray = Fluent("ontray", RealType(), tray=Tray, gluten=Gluten)
    hungry = Fluent("hungry", RealType(), place=Place, gluten=Gluten)

    ### Constants
    kitchen = Object("kitchen", Place)
    gluten_free = Object("is_gluten_free", Gluten)
    not_gluten_free = Object("is_not_gluten_free", Gluten)

    ### Actions
    make_sandwich_no_gluten = InstantaneousAction(
        "make_sandwich_no_gluten", bread_gluten=Gluten, content_gluten=Gluten
    )
    bread_gluten = make_sandwich_no_gluten.parameter("bread_gluten")
    content_gluten = make_sandwich_no_gluten.parameter("content_gluten")
    make_sandwich_no_gluten.add_precondition(is_gluten_free(bread_gluten))
    make_sandwich_no_gluten.add_precondition(is_gluten_free(content_gluten))
    make_sandwich_no_gluten.add_precondition(GE(at_kitchen_bread(bread_gluten), 1))
    make_sandwich_no_gluten.add_precondition(GE(at_kitchen_content(content_gluten), 1))
    make_sandwich_no_gluten.add_decrease_effect(at_kitchen_bread(bread_gluten), 1)
    make_sandwich_no_gluten.add_decrease_effect(at_kitchen_content(content_gluten), 1)
    make_sandwich_no_gluten.add_increase_effect(at_kitchen_sandwich(gluten_free), 1)


    make_sandwich = InstantaneousAction(
        "make_sandwich", bread_gluten=Gluten, content_gluten=Gluten
    )
    bread_gluten = make_sandwich.parameter("bread_gluten")
    content_gluten = make_sandwich.parameter("content_gluten")
    make_sandwich.add_precondition(GE(at_kitchen_bread(bread_gluten), 1))
    make_sandwich.add_precondition(GE(at_kitchen_content(content_gluten), 1))
    make_sandwich.add_decrease_effect(at_kitchen_bread(bread_gluten), 1)
    make_sandwich.add_decrease_effect(at_kitchen_content(content_gluten), 1)
    make_sandwich.add_increase_effect(at_kitchen_sandwich(gluten_free), 1)


    put_on_tray = InstantaneousAction("put_on_tray", tray=Tray, gluten=Gluten)
    tray = put_on_tray.parameter("tray")
    gluten = put_on_tray.parameter("gluten")
    put_on_tray.add_precondition(GE(at_kitchen_sandwich(gluten), 1))
    put_on_tray.add_precondition(at(tray, kitchen))
    put_on_tray.add_decrease_effect(at_kitchen_sandwich(gluten), 1)
    put_on_tray.add_increase_effect(ontray(tray, gluten), 1)


    # must serve gluten_free sandwich to gluten allergic children
    serve_sandwich_no_gluten = InstantaneousAction(
        "serve_sandwich_no_gluten", tray=Tray, place=Place
    )
    tray = serve_sandwich_no_gluten.parameter("tray")
    place = serve_sandwich_no_gluten.parameter("place")
    serve_sandwich_no_gluten.add_precondition(at(tray, place))
    serve_sandwich_no_gluten.add_precondition(GE(ontray(tray, gluten_free), 1))
    serve_sandwich_no_gluten.add_precondition(GE(hungry(place, gluten_free), 1))
    serve_sandwich_no_gluten.add_decrease_effect(ontray(tray, gluten_free), 1)
    serve_sandwich_no_gluten.add_decrease_effect(hungry(place, gluten_free), 1)


    # can serve either gluten or gluten_free sandwich to not gluten allergic children
    serve_sandwich = InstantaneousAction(
        "serve_sandwich", tray=Tray, place=Place, gluten=Gluten
    )
    tray = serve_sandwich.parameter("tray")
    place = serve_sandwich.parameter("place")
    gluten = serve_sandwich.parameter("gluten")
    serve_sandwich.add_precondition(at(tray, place))
    serve_sandwich.add_precondition(GE(ontray(tray, gluten), 1))
    serve_sandwich.add_precondition(GE(hungry(place, not_gluten_free), 1))
    serve_sandwich.add_decrease_effect(ontray(tray, gluten), 1)
    serve_sandwich.add_decrease_effect(hungry(place, not_gluten_free), 1)


    move_tray = InstantaneousAction("move_tray", tray=Tray, place1=Place, place2=Place)
    tray = move_tray.parameter("tray")
    place1 = move_tray.parameter("place1")
    place2 = move_tray.parameter("place2")
    move_tray.add_precondition(at(tray, place1))
    move_tray.add_effect(at(tray, place2), True)
    move_tray.add_effect(at(tray, place1), False)

    def make_problem(p_name):
        problem = Problem(p_name)
        problem.add_fluents([at, is_gluten_free, at_kitchen_bread, at_kitchen_content, at_kitchen_sandwich, ontray, hungry])
        problem.add_actions([make_sandwich_no_gluten, make_sandwich, put_on_tray, serve_sandwich_no_gluten, serve_sandwich, move_tray])
        problem.add_objects([kitchen, gluten_free, not_gluten_free])
        return problem

    problem = make_problem(domain_name)

    writer = PDDLWriter(problem)
    domain_pddl = "numeric/domain.pddl"
    writer.write_domain(domain_pddl)

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
            problem = reader.parse_problem(classic_domain, classic_pddl)
            facts, goals = get_initial_facts_and_goals(problem)
            user_types = {obj.name: obj for obj in problem.user_types}
            fluents = {fluent.name: fluent for fluent in problem.fluents}
            objects = problem.all_objects

            # print(f_name)
            # print(facts)
            # print(goals)

            """""" """""" """""" """""" """""" """""" """""" """""" """""" """"""
            """""" """""" """ actual problem writing starts here """ """""" """"""
            """""" """""" """""" """""" """""" """""" """""" """""" """""" """"""

            n_places = 0
            n_trays = 0
            child_at = {}
            child_allergic = {}

            n_bread = 0
            n_bread_no_gluten = 0
            n_content = 0
            n_content_no_gluten = 0

            for obj in objects:
                if str(obj) != "kitchen" and str(obj.type) == "place":
                    n_places += 1
                elif str(obj.type) == "tray":
                    n_trays += 1

            for fact in facts:
                pred = var_to_predicate(fact)
                objects = var_to_objects(fact)
                if pred == "waiting":
                    child = objects[0]
                    table = int(objects[1].replace("table", "")) - 1
                    child_at[child] = table
                elif pred == "not_allergic_gluten":
                    child = objects[0]
                    child_allergic[child] = False
                elif pred == "allergic_gluten":
                    child = objects[0]
                    child_allergic[child] = True
                elif pred == "at_kitchen_bread":
                    n_bread += 1
                elif pred == "no_gluten_bread":
                    n_bread_no_gluten += 1
                elif pred == "at_kitchen_content":
                    n_content += 1
                elif pred == "no_gluten_content":   
                    n_content_no_gluten += 1
            
            n_bread_gluten = n_bread - n_bread_no_gluten
            n_content_gluten = n_content - n_content_no_gluten
            
            allergic_places = [0 for _ in range(n_places)]
            not_allergic_places = [0 for _ in range(n_places)]

            for child in child_allergic:
                if child_allergic[child]:
                    allergic_places[child_at[child]] += 1
                else:
                    not_allergic_places[child_at[child]] += 1

            print(allergic_places)
            print(not_allergic_places)

            # write domain
            problem_name = f_name.replace(".pddl", "")
            problem = make_problem(problem_name)

            ### objects
            trays = [Object(f"tray{i+1}", Tray) for i in range(n_trays)]
            places = [Object(f"place{i+1}", Place) for i in range(n_places)]
            problem.add_objects(trays + places)

            ### initial state
            problem.set_initial_value(hungry(kitchen, gluten_free), 0)
            for allergic, not_allergic, place in zip(allergic_places, not_allergic_places, places):
                problem.set_initial_value(hungry(place, gluten_free), allergic)
                problem.set_initial_value(hungry(place, not_gluten_free), not_allergic)
            for tray in trays:
                problem.set_initial_value(at(tray, kitchen), True)
                problem.set_initial_value(ontray(tray, gluten_free), 0)
                problem.set_initial_value(ontray(tray, not_gluten_free), 0)

            problem.set_initial_value(at_kitchen_bread(gluten_free), n_bread_no_gluten)
            problem.set_initial_value(at_kitchen_content(gluten_free), n_content_no_gluten)
            problem.set_initial_value(at_kitchen_sandwich(gluten_free), 0)
            problem.set_initial_value(at_kitchen_bread(not_gluten_free), n_bread_gluten)
            problem.set_initial_value(at_kitchen_content(not_gluten_free), n_content_gluten)
            problem.set_initial_value(at_kitchen_sandwich(not_gluten_free), 0)
            problem.set_initial_value(is_gluten_free(gluten_free), True)
            problem.set_initial_value(is_gluten_free(not_gluten_free), False)

            ### goal state
            for place in places:
                problem.add_goal(Equals(hungry(place, gluten_free), 0))
                problem.add_goal(Equals(hungry(place, not_gluten_free), 0))

            ### I/O
            writer = PDDLWriter(problem)
            domain_pddl = "domain.pddl"
            problem_pddl = f"{n_dir}/{f_name}"
            writer.write_problem(problem_pddl)


if __name__ == "__main__":
    main()
