# Import the PDDLReader and PDDLWriter classes
from unified_planning.io import PDDLReader
from unified_planning.shortcuts import SequentialSimulator
from unified_planning.plans import SequentialPlan, ActionInstance
from unified_planning.model.walkers import StateEvaluator
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


def generalize_plan(pddl_problem) -> SequentialPlan:
    plan = list()
    # print(pddl_problem.user_types)  # [child, bread-portion, content-portion, sandwich, tray, place]
    utypes = pddl_problem.user_types
    actions = (
        pddl_problem.actions
    )  # make_sandwich_no_gluten, make_sandwich, put_on_tray, serve_sandwich_no_gluten, serve_sandwich, move_tray
    # print(actions)

    with SequentialSimulator(problem=pddl_problem) as simulator:
        state = simulator.get_initial_state()
        child_objs = [o for o in pddl_problem.objects(utypes[0])]
        bread_objs = [o for o in pddl_problem.objects(utypes[1])]
        content_objs = [o for o in pddl_problem.objects(utypes[2])]
        sandwiches = [o for o in pddl_problem.objects(utypes[3])]
        trays = [o for o in pddl_problem.objects(utypes[4])]
        places = [
            o for o in pddl_problem.objects(utypes[5])
        ]  # kitchen, table1 - 2 - 3 (up to 3 tables)

        # 0. get gluten(-free) food data
        gluten_free_bread = []
        gluten_free_content = []
        num_gluten_free_sandwiches = 0
        for k, v in state._values.items():
            if not v.bool_constant_value():
                continue
            fluent = str(k).split("(")
            if fluent[0] == "no_gluten_bread":
                gluten_free_bread.append(pddl_problem.object(fluent[1][:-1]))
            elif fluent[0] == "no_gluten_content":
                gluten_free_content.append(pddl_problem.object(fluent[1][:-1]))
            elif fluent[0] == "allergic_gluten":
                num_gluten_free_sandwiches += 1

        # max_gluten_free_sandwiches = min(len(gluten_free_content), len(gluten_free_bread))
        gluten_free_sandwiches = sandwiches[0:num_gluten_free_sandwiches]
        gluten_sandwiches = sandwiches[num_gluten_free_sandwiches:]
        gluten_bread = [
            bread for bread in bread_objs if not (bread in gluten_free_bread)
        ] + gluten_free_bread[num_gluten_free_sandwiches:]
        gluten_content = [
            content
            for content in content_objs
            if not (content in gluten_free_content)
        ] + gluten_free_content[num_gluten_free_sandwiches:]

        # 1. make all possible gluten-free sandwiches
        made_sandwiches = []
        for idx in range(0, num_gluten_free_sandwiches):
            make_sand_no_gluten = ActionInstance(
                actions[0],
                tuple(
                    [
                        gluten_free_sandwiches[idx],
                        gluten_free_bread[idx],
                        gluten_free_content[idx],
                    ]
                ),
            )
            assert simulator.is_applicable(state, make_sand_no_gluten)
            state = simulator.apply(state, make_sand_no_gluten)
            made_sandwiches.append(gluten_free_sandwiches[idx])
            plan.append(make_sand_no_gluten)

        # 2. make the rest of sandwiches with gluten
        num_gluten_sandwiches = min(
            len(gluten_sandwiches), len(gluten_bread), len(gluten_content)
        )
        for idx in range(0, num_gluten_sandwiches):
            make_sand = ActionInstance(
                actions[1],
                tuple(
                    [
                        gluten_sandwiches[idx],
                        gluten_bread[idx],
                        gluten_content[idx],
                    ]
                ),
            )
            assert simulator.is_applicable(state, make_sand)
            state = simulator.apply(state, make_sand)
            made_sandwiches.append(gluten_sandwiches[idx])
            plan.append(make_sand)

        # 3. put them all on a tray
        for s in made_sandwiches:
            put_on_tray = ActionInstance(
                actions[2], tuple([s, trays[0]])
            )  # use always the first tray
            assert simulator.is_applicable(state, put_on_tray)
            state = simulator.apply(state, put_on_tray)
            plan.append(put_on_tray)

        # 4. move the tray starting in the kitchen, then to each table, while serving all sandwiches
        current_gluten_sand, current_gluten_free_sand = 0, 0
        tray_at = pddl_problem.object("kitchen")
        served = set()
        for idx in range(1, len(places)):  # first place is always the kitchen
            next_at = places[idx]
            move = ActionInstance(
                actions[5], tuple([trays[0], tray_at, next_at])
            )
            assert simulator.is_applicable(state, move)
            state = simulator.apply(state, move)
            plan.append(move)
            tray_at = next_at
            for child in child_objs:
                if child in served:
                    continue
                if current_gluten_sand < len(
                    gluten_sandwiches
                ):  # a gluten sand can be served
                    serve_gluten_sand = ActionInstance(
                        actions[4],
                        tuple(
                            [
                                gluten_sandwiches[current_gluten_sand],
                                child,
                                trays[0],
                                tray_at,
                            ]
                        ),
                    )
                    if simulator.is_applicable(state, serve_gluten_sand):
                        state = simulator.apply(state, serve_gluten_sand)
                        served.add(child)
                        current_gluten_sand += 1
                        plan.append(serve_gluten_sand)
                        continue
                if current_gluten_free_sand < len(
                    gluten_free_sandwiches
                ):  # a gluten free sand can be served
                    serve_gluten_free_sand = ActionInstance(
                        actions[3],
                        tuple(
                            [
                                gluten_free_sandwiches[
                                    current_gluten_free_sand
                                ],
                                child,
                                trays[0],
                                tray_at,
                            ]
                        ),
                    )  # gluten-free
                    if simulator.is_applicable(state, serve_gluten_free_sand):
                        state = simulator.apply(state, serve_gluten_free_sand)
                        served.add(child)
                        current_gluten_free_sand += 1
                        plan.append(serve_gluten_free_sand)

    return SequentialPlan(plan)


def main():
    os.makedirs("training/easy/", exist_ok=True)
    os.makedirs("testing/easy/", exist_ok=True)
    os.makedirs("testing/medium/", exist_ok=True)
    os.makedirs("testing/hard/", exist_ok=True)

    reader = PDDLReader()
    """
    pddl_problem = reader.parse_problem('../../childsnack/domain.pddl', '../../childsnack/training/easy/p05.pddl')
    plan = generalize_plan(pddl_problem)
    print(plan)
    print(f"Is valid? {apply_plan(pddl_problem, plan)}")
    """
    all_problems = [
        f"../../childsnack/training/easy/p{p:02}.pddl" for p in range(1, 100)
    ]
    all_problems.extend(
        [f"../../childsnack/testing/easy/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [f"../../childsnack/testing/medium/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [f"../../childsnack/testing/hard/p{p:02}.pddl" for p in range(1, 31)]
    )

    all_plans = [f"training/easy/p{p:02}.plan" for p in range(1, 100)]
    all_plans.extend([f"testing/easy/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/medium/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/hard/p{p:02}.plan" for p in range(1, 31)])

    for prob, plan_file in zip(all_problems, all_plans):
        print(f"Solving {prob}...")
        pddl_problem = reader.parse_problem(
            "../../childsnack/domain.pddl", prob
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
