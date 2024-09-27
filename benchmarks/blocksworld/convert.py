import argparse
import math
import os
import random

from tqdm import tqdm
from unified_planning.io import PDDLWriter
from unified_planning.io.pddl_reader import PDDLReader
from unified_planning.shortcuts import *

"""
Simplified encoding of Hydraulic Blocks World from [Haslum et al., JAIR-18] with no
more hydraulic interactions which is too annoying to model with just PDDL.
Now, each cylinder has a fixed weight support, instead of having a height which
effects every other cylinder's height.
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
    ### Types
    Obj = UserType("Obj")
    Block = UserType("Block", father=Obj)
    Cylinder = UserType("Cylinder", father=Obj)

    ### Variables
    # Boolean
    base = Fluent("base", BoolType(), block=Block)  # bottom of a tower
    on = Fluent("on", BoolType(), block=Block, support=Obj)  # on block or cylinder
    inside = Fluent("in", BoolType(), block=Block, cylinder=Cylinder)
    clear = Fluent("clear", BoolType(), obj=Obj)
    holding = Fluent("holding", BoolType(), block=Block)
    arm_empty = Fluent("arm_empty", BoolType())

    # Numeric
    capacity = Fluent("capacity", RealType(), cylinder=Cylinder)  # static

    ### Actions
    pickup = InstantaneousAction("pickup", block=Block, cylinder=Cylinder)
    block = pickup.parameter("block")
    cylinder = pickup.parameter("cylinder")
    pickup.add_precondition(base(block))
    pickup.add_precondition(on(block, cylinder))
    pickup.add_precondition(inside(block, cylinder))
    pickup.add_precondition(clear(block))
    pickup.add_precondition(arm_empty())
    pickup.add_effect(base(block), False)
    pickup.add_effect(on(block, cylinder), False)
    pickup.add_effect(inside(block, cylinder), False)
    pickup.add_effect(clear(block), False)
    pickup.add_effect(clear(cylinder), True)
    pickup.add_effect(holding(block), True)
    pickup.add_effect(arm_empty(), False)
    pickup.add_increase_effect(capacity(cylinder), 1)

    putdown = InstantaneousAction("putdown", block=Block, cylinder=Cylinder)
    block = putdown.parameter("block")
    cylinder = putdown.parameter("cylinder")
    putdown.add_precondition(holding(block))
    putdown.add_precondition(clear(cylinder))
    putdown.add_precondition(GE(capacity(cylinder), 1))
    putdown.add_effect(base(block), True)
    putdown.add_effect(holding(block), False)
    putdown.add_effect(clear(cylinder), False)
    putdown.add_effect(on(block, cylinder), True)
    putdown.add_effect(inside(block, cylinder), True)
    putdown.add_effect(clear(block), True)
    putdown.add_effect(arm_empty(), True)
    putdown.add_decrease_effect(capacity(cylinder), 1)

    unstack = InstantaneousAction(
        "unstack", block_a=Block, block_b=Block, cylinder=Cylinder
    )
    block_a = unstack.parameter("block_a")
    block_b = unstack.parameter("block_b")
    cylinder = unstack.parameter("cylinder")
    unstack.add_precondition(on(block_a, block_b))
    unstack.add_precondition(inside(block_a, cylinder))
    unstack.add_precondition(clear(block_a))
    unstack.add_precondition(arm_empty())
    unstack.add_effect(on(block_a, block_b), False)
    unstack.add_effect(inside(block_a, cylinder), False)
    unstack.add_effect(clear(block_a), False)
    unstack.add_effect(clear(block_b), True)
    unstack.add_effect(holding(block_a), True)
    unstack.add_effect(arm_empty(), False)
    unstack.add_increase_effect(capacity(cylinder), 1)

    stack = InstantaneousAction(
        "stack", block_a=Block, block_b=Block, cylinder=Cylinder
    )
    block_a = stack.parameter("block_a")
    block_b = stack.parameter("block_b")
    cylinder = stack.parameter("cylinder")
    stack.add_precondition(holding(block_a))
    stack.add_precondition(clear(block_b))
    stack.add_precondition(inside(block_b, cylinder))
    stack.add_precondition(GE(capacity(cylinder), 1))
    stack.add_effect(holding(block_a), False)
    stack.add_effect(clear(block_b), False)
    stack.add_effect(on(block_a, block_b), True)
    stack.add_effect(inside(block_a, cylinder), True)
    stack.add_effect(clear(block_a), True)
    stack.add_effect(arm_empty(), True)
    stack.add_decrease_effect(capacity(cylinder), 1)

    problem_name = f"blocksworld_numeric"
    problem = Problem(problem_name)
    problem.add_fluents(
        [
            base,
            on,
            inside,
            clear,
            holding,
            arm_empty,
            capacity,
        ]
    )
    problem.add_actions([pickup, putdown, unstack, stack])

    writer = PDDLWriter(problem)
    domain_pddl = "numeric/domain.pddl"
    writer.write_domain(domain_pddl)

    """""" """""" """""" """""" """""" """""" """""" """""" """""" """""" """"""
    """""" """""" """ actual problem writing starts here """ """""" """"""
    """""" """""" """""" """""" """""" """""" """""" """""" """""" """""" """"""

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

            init_state = {}  # below_block: above_block
            goal_state = {}

            n_init_towers = 0
            n_goal_towers = 0

            for fact in facts:
                predicate = var_to_predicate(fact)
                objects = var_to_objects(fact)
                if predicate == "on":
                    top = int(objects[0].replace("b", ""))
                    bottom = int(objects[1].replace("b", ""))
                    init_state[bottom] = top
                elif predicate == "on-table":
                    top = int(objects[0].replace("b", ""))
                    bottom = f"c{n_init_towers}"
                    n_init_towers += 1
                    init_state[bottom] = top

            for fact in goals:
                predicate = var_to_predicate(fact)
                objects = var_to_objects(fact)
                if predicate == "on":
                    top = int(objects[0].replace("b", ""))
                    bottom = int(objects[1].replace("b", ""))
                    goal_state[bottom] = top
                elif predicate == "on-table":
                    top = int(objects[0].replace("b", ""))
                    bottom = f"c{n_goal_towers}"
                    n_goal_towers += 1
                    goal_state[bottom] = top

            n_blocks = len(problem.all_objects)

            init_towers = [[] for _ in range(n_init_towers)]
            goal_towers = [[] for _ in range(n_goal_towers)]
            for above, towers in [(init_state, init_towers), (goal_state, goal_towers)]:
                tower_of_block = {}
                added = set()
                for k in above:
                    q = [k]
                    while q:
                        below_obj = q.pop(0)
                        if below_obj not in above:
                            continue
                        above_obj = above[below_obj]
                        if above_obj in added:
                            continue
                        if isinstance(below_obj, str):
                            tower = int(below_obj.replace("c", ""))
                            towers[tower].append(above_obj)
                            tower_of_block[above_obj] = tower
                        else:
                            assert isinstance(below_obj, int)
                            if below_obj not in tower_of_block:
                                # print(below_obj, above_obj)
                                continue  # maybe find it later?
                            tower = tower_of_block[below_obj]
                            towers[tower].append(above_obj)
                            tower_of_block[above_obj] = tower

                        added.add(above_obj)
                        q.append(above_obj)

            init_towers = sorted(init_towers, key=lambda x: len(x))
            goal_towers = sorted(goal_towers, key=lambda x: len(x))

            n_cylinders = max(len(init_towers), len(goal_towers))
            n_cylinders = max(4, int(math.ceil(n_cylinders * 1.25)))

            # write domain
            problem_name = f_name.replace(".pddl", "")
            problem = Problem(problem_name)
            problem.add_fluents(
                [
                    base,
                    on,
                    inside,
                    clear,
                    holding,
                    arm_empty,
                    capacity,
                ]
            )
            problem.add_actions([pickup, putdown, unstack, stack])

            # objects
            blocks = [Object(f"b{i+1}", Block) for i in range(n_blocks)]
            cylinders = [Object(f"c{i+1}", Cylinder) for i in range(n_cylinders)]
            problem.add_objects(blocks + cylinders)
            problem.set_initial_value(arm_empty(), True)

            blocks = [""] + blocks  # annoying 1- and 0- indexing problems otherwise

            # max cylinder capacity
            tower_heights = []
            for i in range(n_cylinders):
                init_tower_height = len(init_towers[i] if i < len(init_towers) else [])
                goal_tower_height = len(goal_towers[i] if i < len(goal_towers) else [])
                max_height = max(init_tower_height, goal_tower_height)
                tower_heights.append(max_height)
            B = int(2.5 * n_blocks)
            blocks_to_add = B - sum(tower_heights)

            min_tower_height = min(tower_heights)
            
            cnt = -1
            while blocks_to_add > 0:
                cnt += 1
                cnt %= n_cylinders
                if tower_heights[cnt] > min_tower_height:
                    continue
                tower_heights[cnt] += 1
                blocks_to_add -= 1
                min_tower_height = min(tower_heights)

            assert sum(tower_heights) == B, (sum(tower_heights), B)
            print(f_name, tower_heights)

            for i in range(n_cylinders):
                problem.set_initial_value(capacity(cylinders[i]), tower_heights[i] - (len(init_towers[i]) if i < len(init_towers) else 0))

            # initial state
            for i, tower in enumerate(init_towers):
                cylinder = cylinders[i]
                bottom_block = blocks[tower[0]]
                problem.set_initial_value(base(bottom_block), True)
                problem.set_initial_value(inside(bottom_block, cylinder), True)
                problem.set_initial_value(on(bottom_block, cylinder), True)
                for j in range(1, len(tower)):
                    top_block = blocks[tower[j]]
                    bot_block = blocks[tower[j - 1]]
                    problem.set_initial_value(on(top_block, bot_block), True)
                    problem.set_initial_value(inside(top_block, cylinder), True)
                problem.set_initial_value(clear(blocks[tower[-1]]), True)

            # fill in remaining cylinders
            for i in range(len(init_towers), n_cylinders):
                problem.set_initial_value(clear(cylinders[i]), True)

            # goal state
            for i, tower in enumerate(goal_towers):
                cylinder = cylinders[i]
                bottom_block = blocks[tower[0]]
                problem.add_goal(on(bottom_block, cylinder))
                for j in range(1, len(tower)):
                    top_block = blocks[tower[j]]
                    bot_block = blocks[tower[j - 1]]
                    problem.add_goal(on(top_block, bot_block))
                problem.add_goal(clear(blocks[tower[-1]]))

            # I/O
            writer = PDDLWriter(problem)
            domain_pddl = "domain.pddl"
            problem_pddl = f"{n_dir}/{f_name}"
            writer.write_problem(problem_pddl)


if __name__ == "__main__":
    main()
