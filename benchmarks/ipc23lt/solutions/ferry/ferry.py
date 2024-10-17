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
    # print(pddl_problem.user_types)  # [car, location]
    utypes = pddl_problem.user_types
    actions = pddl_problem.actions  # sail, board, debark
    # print(actions)
    car_objs = [o for o in pddl_problem.objects(utypes[0])]
    # print(car_objs)
    # location_objs = [o for o in pddl_problem.objects(utypes[1])]
    # print(location_objs)
    goal_fluents = [g for g in pddl_problem.goals[0].args]
    assert goal_fluents
    if goal_fluents[0].is_object_exp():
        goal_fluents = [
            g for g in pddl_problem.goals
        ]  # single fluent in the goal

    g_car = {
        str(g.args[0]): g.args[1] for g in goal_fluents
    }  # all are at(car,loc) fluents in the goal

    with SequentialSimulator(problem=pddl_problem) as simulator:
        state = simulator.get_initial_state()

        # 0. Get the initial ferry and car locations
        i_car = dict()
        for k, v in state._values.items():
            # add it, if it is "at" predicate (the only with two arguments) and true valued
            if (v.bool_constant_value()) and (len(k.args) == 2):
                i_car[str(k.args[0])] = k.args[1]
            elif (v.bool_constant_value()) and str(k)[
                0:3
            ] == "at-":  # get the first ferry location
                at_ferry = k.args[0]

        # 1. Repeat the macro, sail to i-th car location, board, go to its goal location, debark, for each car
        for car in car_objs:
            dest = i_car[str(car)]
            sail = ActionInstance(actions[0], tuple([at_ferry, dest]))
            if simulator.is_applicable(state, sail):
                state = simulator.apply(state, sail)
                at_ferry = dest
                plan.extend([sail])

            board = ActionInstance(actions[1], tuple([car, dest]))
            assert simulator.is_applicable(state, board)
            state = simulator.apply(state, board)

            dest = g_car[str(car)]
            sail = ActionInstance(actions[0], tuple([at_ferry, dest]))
            assert simulator.is_applicable(state, sail)
            state = simulator.apply(state, sail)
            at_ferry = dest

            debark = ActionInstance(actions[2], tuple([car, dest]))
            assert simulator.is_applicable(state, debark)
            state = simulator.apply(state, debark)

            plan.extend([board, sail, debark])

    return SequentialPlan(plan)


def main():
    os.makedirs("training/easy/", exist_ok=True)
    os.makedirs("testing/easy/", exist_ok=True)
    os.makedirs("testing/medium/", exist_ok=True)
    os.makedirs("testing/hard/", exist_ok=True)

    reader = PDDLReader()
    """
    pddl_problem = reader.parse_problem('../../ferry/domain.pddl', '../../ferry/training/easy/p04.pddl')
    plan = generalize_plan(pddl_problem)
    print(plan)
    print(f"Is valid? {apply_plan(pddl_problem, plan)}")
    """
    all_problems = [
        f"../../ferry/training/easy/p{p:02}.pddl" for p in range(1, 100)
    ]
    all_problems.extend(
        [f"../../ferry/testing/easy/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [f"../../ferry/testing/medium/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [f"../../ferry/testing/hard/p{p:02}.pddl" for p in range(1, 31)]
    )

    all_plans = [f"training/easy/p{p:02}.plan" for p in range(1, 100)]
    all_plans.extend([f"testing/easy/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/medium/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/hard/p{p:02}.plan" for p in range(1, 31)])

    for prob, plan_file in zip(all_problems, all_plans):
        print(f"Solving {prob}...")
        pddl_problem = reader.parse_problem("../../ferry/domain.pddl", prob)
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
