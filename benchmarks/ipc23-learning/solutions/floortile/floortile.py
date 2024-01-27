# Import the PDDLReader and PDDLWriter classes
from unified_planning.io import PDDLReader
from unified_planning.shortcuts import SequentialSimulator
from unified_planning.plans import SequentialPlan, ActionInstance
from unified_planning.model.walkers import StateEvaluator
from typing import Tuple
import os


def apply_plan(pddl_problem, plan) -> bool:
    goal = pddl_problem.goals[0]

    with SequentialSimulator(problem=pddl_problem) as simulator:
        state = simulator.get_initial_state()
        se = StateEvaluator(simulator._problem)
        for act in plan.actions:
            if simulator.is_applicable(state, act):
                state = simulator.apply(state, act)
        if se.evaluate(goal, state).bool_constant_value():
            print("Goal achieved!")
            return True

        print("Invalid plan :-(")
        return False


def get_row_col(tile_name: str) -> Tuple[int, int]:
    _, r, c = tile_name.split("_")
    return tuple((int(r), int(c)))


def generalize_plan(pddl_problem) -> SequentialPlan:
    plan = list()
    # print(pddl_problem.user_types)  # [robot, tile, color]
    utypes = pddl_problem.user_types
    actions = (
        pddl_problem.actions
    )  # change_color, paint_up, paint_down, move_up, move_down, move_right, move_left
    # print(actions)
    # robot_objs = [o for o in pddl_problem.objects(utypes[0])]
    tile_objs = set([str(o) for o in pddl_problem.objects(utypes[1])])
    white_obj = pddl_problem.object("white")
    black_obj = pddl_problem.object("black")
    max_rows, max_cols = max(
        [get_row_col(t) for t in tile_objs]
    )  # get rows and cols

    with SequentialSimulator(problem=pddl_problem) as simulator:
        state = simulator.get_initial_state()
        robots_at = dict()
        tile_robots = []

        # 0. get all robot locations and sort them by column
        for k, v in state._values.items():
            if not v.bool_constant_value():
                continue
            fluent = str(k).split("(")
            if fluent[0] == "robot-at":  # get where the lift is at
                robots_at[k.args[0]] = k.args[1]
                r, c = get_row_col(str(k.args[1]))
                tile_robots.append((c, k.args[0]))

        sorted_robots = [r for _, r in sorted(tile_robots)]
        # print(sorted_robots)
        # 1. move all robots to the upmost tile in their current column
        prev_col = 0
        for r in sorted_robots:
            tile = robots_at[r]
            current_row, current_col = get_row_col(str(tile))
            # 1.a first to the topmost tile
            while current_row < max_rows:
                next_tile = pddl_problem.object(
                    f"tile_{current_row+1}_{current_col}"
                )
                move = ActionInstance(
                    actions[3], tuple([r, tile, next_tile])
                )  # move up
                assert simulator.is_applicable(state, move)
                state = simulator.apply(state, move)
                plan.append(move)
                tile = next_tile
                current_row += 1

            # 1.b then to the leftmost
            while prev_col + 1 < current_col:
                next_tile = pddl_problem.object(
                    f"tile_{current_row}_{current_col-1}"
                )
                move = ActionInstance(
                    actions[6], tuple([r, tile, next_tile])
                )  # move left
                assert simulator.is_applicable(state, move)
                state = simulator.apply(state, move)
                plan.append(move)
                tile = next_tile
                current_col -= 1
            robots_at[r] = tile
            prev_col += 1

        # 2. set the corresponding color, if (r*c) is odd then white, otherwise black
        robot_color = []
        for r in sorted_robots:
            tile = robots_at[r]
            row, col = get_row_col(str(tile))
            if ((row + col) % 2) == 0:  # odd
                change_white = ActionInstance(
                    actions[0], tuple([r, black_obj, white_obj])
                )
                if simulator.is_applicable(state, change_white):
                    state = simulator.apply(state, change_white)
                    plan.append(change_white)
                robot_color.append(white_obj)
            else:  # even
                change_black = ActionInstance(
                    actions[0], tuple([r, white_obj, black_obj])
                )
                if simulator.is_applicable(state, change_black):
                    state = simulator.apply(state, change_black)
                    plan.append(change_black)
                robot_color.append(black_obj)

        # 3. move all down, paint up, and change color
        def move_down_and_paint_up(state, idx, robot):
            local_plan = []
            tile = robots_at[robot]
            row, col = get_row_col(str(tile))
            while row > 0:
                next_tile = pddl_problem.object(f"tile_{row - 1}_{col}")
                move_down = ActionInstance(
                    actions[4], tuple([robot, tile, next_tile])
                )  # move down
                assert simulator.is_applicable(state, move_down)
                state = simulator.apply(state, move_down)
                local_plan.append(move_down)

                paint_up = ActionInstance(
                    actions[1],
                    tuple([robot, tile, next_tile, robot_color[idx]]),
                )  # paint up
                # print(paint_up)
                # print(state)
                assert simulator.is_applicable(state, paint_up)
                state = simulator.apply(state, paint_up)
                local_plan.append(paint_up)

                # change either to white or black
                if robot_color[idx] == white_obj:
                    change_black = ActionInstance(
                        actions[0], tuple([robot, white_obj, black_obj])
                    )
                    assert simulator.is_applicable(state, change_black)
                    state = simulator.apply(state, change_black)
                    local_plan.append(change_black)
                    robot_color[idx] = black_obj
                else:
                    change_white = ActionInstance(
                        actions[0], tuple([robot, black_obj, white_obj])
                    )
                    assert simulator.is_applicable(state, change_white)
                    state = simulator.apply(state, change_white)
                    local_plan.append(change_white)
                    robot_color[idx] = white_obj

                tile = next_tile
                row -= 1

            return state, tile, local_plan

        for idx, r in enumerate(sorted_robots):
            state, tile, local_plan = move_down_and_paint_up(state, idx, r)
            robots_at[r] = tile
            plan.extend(local_plan)

        # 4. move the rightmost robot to the right, then to the top, and paint (repeat until last column)
        robot = sorted_robots[-1]
        tile = robots_at[robot]
        row, col = get_row_col(str(tile))
        while col < max_cols:
            # print(col, max_cols)
            # 4.a move to the right once
            next_tile = pddl_problem.object(f"tile_{row}_{col+1}")
            move_right = ActionInstance(
                actions[5], tuple([robot, tile, next_tile])
            )
            assert simulator.is_applicable(state, move_right)
            state = simulator.apply(state, move_right)
            plan.append(move_right)
            tile = next_tile
            col += 1
            robots_at[robot] = tile

            # 4.b move to the topmost tile in the column
            while row < max_rows:
                next_tile = pddl_problem.object(f"tile_{row+1}_{col}")
                move_up = ActionInstance(
                    actions[3], tuple([robot, tile, next_tile])
                )
                assert simulator.is_applicable(state, move_up)
                state = simulator.apply(state, move_up)
                plan.append(move_up)
                tile = next_tile
                row += 1
            robots_at[robot] = tile

            # 4.c change to the right color
            if ((row + col) % 2) == 0:  # odd
                change_white = ActionInstance(
                    actions[0], tuple([robot, black_obj, white_obj])
                )
                if simulator.is_applicable(state, change_white):
                    state = simulator.apply(state, change_white)
                    plan.append(change_white)
                robot_color[-1] = white_obj
            else:  # even
                change_black = ActionInstance(
                    actions[0], tuple([robot, white_obj, black_obj])
                )
                if simulator.is_applicable(state, change_black):
                    state = simulator.apply(state, change_black)
                    plan.append(change_black)
                robot_color[-1] = black_obj

            # 4.d move down and paint up while changing color
            state, tile, local_plan = move_down_and_paint_up(
                state, len(sorted_robots) - 1, robot
            )
            row, col = get_row_col(str(tile))
            robots_at[robot] = tile
            plan.extend(local_plan)

    return SequentialPlan(plan)


def main():
    os.makedirs("training/easy/", exist_ok=True)
    os.makedirs("testing/easy/", exist_ok=True)
    os.makedirs("testing/medium/", exist_ok=True)
    os.makedirs("testing/hard/", exist_ok=True)

    reader = PDDLReader()
    """
    pddl_problem = reader.parse_problem('../../floortile/domain.pddl', '../../floortile/training/easy/p88.pddl')
    plan = generalize_plan(pddl_problem)
    print(plan)
    print(f"Is valid? {apply_plan(pddl_problem, plan)}")
    """
    all_problems = [
        f"../../floortile/training/easy/p{p:02}.pddl" for p in range(1, 100)
    ]
    all_problems.extend(
        [f"../../floortile/testing/easy/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [f"../../floortile/testing/medium/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [f"../../floortile/testing/hard/p{p:02}.pddl" for p in range(1, 31)]
    )

    all_plans = [f"training/easy/p{p:02}.plan" for p in range(1, 100)]
    all_plans.extend([f"testing/easy/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/medium/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/hard/p{p:02}.plan" for p in range(1, 31)])

    for prob, plan_file in zip(all_problems, all_plans):
        print(f"Solving {prob}...")
        pddl_problem = reader.parse_problem(
            "../../floortile/domain.pddl", prob
        )
        plan = generalize_plan(pddl_problem)
        # if not apply_plan(pddl_problem, plan):
        #    print(f"Problem {prob} failed!")
        # else:
        #    with open(plan_file, "w") as f:
        #        f.write(plan.__str__())
        with open(plan_file, "w") as f:
            for act in plan._actions:
                f.write(
                    f"({act._action._name} {' '.join([str(arg) for arg in act._params])})\n"
                )
            f.write(f"; cost = {len(plan._actions)} (unit cost)")


# """


if __name__ == "__main__":
    main()
