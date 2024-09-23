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
    # print(pddl_problem.user_types)  # [location, locatable, man - locatable, nut - locatable, spanner - locatable]
    utypes = pddl_problem.user_types
    actions = pddl_problem.actions  # walk, pickup_spanner, tighten_nut
    # print(actions)
    man_objs = [o for o in pddl_problem.objects(utypes[2])]
    # print(man_objs)
    location_objs = [o for o in pddl_problem.objects(utypes[0])]
    # print(location_objs)
    spanner_objs = [o for o in pddl_problem.objects(utypes[4])]
    # print(spanner_objs)
    nut_objs = [o for o in pddl_problem.objects(utypes[3])]
    # print(nut_objs)

    with SequentialSimulator(problem=pddl_problem) as simulator:
        state = simulator.get_initial_state()
        # 1. walk to the next location and collect all spanners
        for idx in range(0, len(location_objs) - 1):
            walk = ActionInstance(
                actions[0],
                tuple(
                    [location_objs[idx], location_objs[idx + 1], man_objs[0]]
                ),
            )
            assert simulator.is_applicable(state, walk)
            state = simulator.apply(state, walk)
            plan.append(walk)
            for sp in spanner_objs:
                pickup = ActionInstance(
                    actions[1],
                    tuple([location_objs[idx + 1], sp, man_objs[0]]),
                )
                if simulator.is_applicable(state, pickup):
                    state = simulator.apply(state, pickup)
                    plan.append(pickup)

        # 2. tighten all nuts
        for idx, nut in enumerate(nut_objs):
            tighten = ActionInstance(
                actions[2],
                tuple(
                    [location_objs[-1], spanner_objs[idx], man_objs[0], nut]
                ),
            )
            assert simulator.is_applicable(state, tighten)
            state = simulator.apply(state, tighten)
            plan.append(tighten)

    return SequentialPlan(plan)


def main():
    os.makedirs("training/easy/", exist_ok=True)
    os.makedirs("testing/easy/", exist_ok=True)
    os.makedirs("testing/medium/", exist_ok=True)
    os.makedirs("testing/hard/", exist_ok=True)

    reader = PDDLReader()
    """
    pddl_problem = reader.parse_problem('../../spanner/domain.pddl', '../../spanner/training/easy/p01.pddl')
    plan = generalize_plan(pddl_problem)
    print(plan)
    print(f"Is valid? {apply_plan(pddl_problem, plan)}")
    """
    all_problems = [
        f"../../spanner/training/easy/p{p:02}.pddl" for p in range(1, 100)
    ]
    all_problems.extend(
        [f"../../spanner/testing/easy/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [f"../../spanner/testing/medium/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [f"../../spanner/testing/hard/p{p:02}.pddl" for p in range(1, 31)]
    )

    all_plans = [f"training/easy/p{p:02}.plan" for p in range(1, 100)]
    all_plans.extend([f"testing/easy/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/medium/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/hard/p{p:02}.plan" for p in range(1, 31)])

    for prob, plan_file in zip(all_problems, all_plans):
        print(f"Solving {prob}...")
        pddl_problem = reader.parse_problem("../../spanner/domain.pddl", prob)
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
