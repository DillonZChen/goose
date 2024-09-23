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

    actions = pddl_problem.actions  # pickup, putdown, stack, unstack
    objs = [o for o in pddl_problem.all_objects]
    goal_fluents = [g for g in pddl_problem.goals[0].args]
    # print(goal_fluents)

    g_clear = set(
        [
            str(g).replace(" ", "")
            for g in goal_fluents
            if "clear" in g.get_contained_names()
        ]
    )
    g_on = set(
        [
            str(g).replace(" ", "")
            for g in goal_fluents
            if "on" in g.get_contained_names()
        ]
    )
    g_ontable = set(
        [
            str(g).replace(" ", "")
            for g in goal_fluents
            if "on-table" in g.get_contained_names()
        ]
    )
    # print(g_clear, g_on, g_ontable)
    well_placed = [f"on-table({o})" in g_ontable for o in objs]
    # well_placed_clear = [f"clear({o})" in g_clear for o in objs]
    # print(well_placed)

    with SequentialSimulator(problem=pddl_problem) as simulator:
        state = simulator.get_initial_state()
        # 1. Put all blocks onto the table
        for _ in objs:
            for o2 in objs:
                for o3 in objs:
                    unstack = ActionInstance(
                        action=actions[3], params=tuple([o2, o3])
                    )  # unstack(?o2,?o3)
                    if simulator.is_applicable(state, unstack):
                        state = simulator.apply(state, unstack)
                        putdown = ActionInstance(
                            action=actions[1], params=tuple([o2])
                        )  # putdown(?o2)
                        state = simulator.apply(state, putdown)
                        plan.extend([unstack, putdown])

        # 2. Increase well-placed blocks by picking-up a not well-placed block ?b1 from the table,
        # and stacking it onto the corresponding well-placed block ?b2, such that (on ?b1 ?b2) is in the goal
        for _ in objs:
            for idx2, o2 in enumerate(objs):
                if well_placed[idx2]:
                    continue
                for idx3, o3 in enumerate(objs):
                    if well_placed[idx3] and (f"on({o2},{o3})" in g_on):
                        pickup = ActionInstance(
                            action=actions[0], params=tuple([o2])
                        )  # pickup(?o2)
                        assert simulator.is_applicable(state, pickup)
                        state = simulator.apply(state, pickup)
                        stack = ActionInstance(
                            action=actions[2], params=tuple([o2, o3])
                        )  # stack(?o2,?o3)
                        assert simulator.is_applicable(state, stack)
                        state = simulator.apply(state, stack)
                        plan.extend([pickup, stack])
                        well_placed[idx2] = True
                        break

    return SequentialPlan(plan)


def plan_test():
    reader = PDDLReader()
    pddl_problem = reader.parse_problem(
        "../blocksworld/domain.pddl", "../blocksworld/training/easy/p01.pddl"
    )
    # print(pddl_problem)
    actions = pddl_problem.actions  # pickup, putdown, stack, unstack

    objs = [o for o in pddl_problem.all_objects]  # b1 b2
    # print(objs[1])
    # print(pddl_problem.object("b1"))
    plan = SequentialPlan(
        [
            ActionInstance(action=actions[0], params=tuple([objs[0]])),
            ActionInstance(
                action=actions[2], params=tuple([objs[0], objs[1]])
            ),
        ]
    )

    return apply_plan(pddl_problem=pddl_problem, plan=plan)


def main():
    os.makedirs("training/easy/", exist_ok=True)
    os.makedirs("testing/easy/", exist_ok=True)
    os.makedirs("testing/medium/", exist_ok=True)
    os.makedirs("testing/hard/", exist_ok=True)

    reader = PDDLReader()
    all_problems = [
        f"../../blocksworld/training/easy/p{p:02}.pddl" for p in range(1, 100)
    ]
    all_problems.extend(
        [f"../../blocksworld/testing/easy/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [
            f"../../blocksworld/testing/medium/p{p:02}.pddl"
            for p in range(1, 31)
        ]
    )
    all_problems.extend(
        [f"../../blocksworld/testing/hard/p{p:02}.pddl" for p in range(1, 31)]
    )

    all_plans = [f"training/easy/p{p:02}.plan" for p in range(1, 100)]
    all_plans.extend([f"testing/easy/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/medium/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/hard/p{p:02}.plan" for p in range(1, 31)])

    for prob, plan_file in zip(all_problems, all_plans):
        print(f"Solving {prob}...")
        pddl_problem = reader.parse_problem(
            "../../blocksworld/domain.pddl", prob
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


if __name__ == "__main__":
    main()
