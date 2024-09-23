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
    # print(pddl_problem.user_types)  # [satellite, direction, instrument, mode]
    utypes = pddl_problem.user_types
    actions = (
        pddl_problem.actions
    )  # 0: turn_to, 1: switch_on, 2: switch_off, 3: calibrate, 4: take_image
    # print(actions)

    # Objects
    sat_objs = [o for o in pddl_problem.objects(utypes[0])]
    dir_objs = [o for o in pddl_problem.objects(utypes[1])]
    ins_objs = [o for o in pddl_problem.objects(utypes[2])]
    mod_objs = [o for o in pddl_problem.objects(utypes[3])]

    # Goal fluents
    goal_fluents = [g for g in pddl_problem.goals[0].args]
    assert goal_fluents
    if goal_fluents[0].is_object_exp():
        goal_fluents = [
            g for g in pddl_problem.goals
        ]  # single fluent in the goal

    # get all are pointing(sat,dir) fluents in the goal
    g_sat = {
        str(g.args[0]): pddl_problem.object(str(g.args[1]))
        for g in goal_fluents
        if str(g).split("(")[0] == "pointing"
    }

    with SequentialSimulator(problem=pddl_problem) as simulator:
        state = simulator.get_initial_state()
        satellites_at = dict()
        calibration_targets = dict()

        # 0. get all satellite directions
        for k, v in state._values.items():
            if not v.bool_constant_value():
                continue
            fluent = str(k).split("(")
            if fluent[0] == "pointing":  # get where the satellite is at
                satellites_at[str(k.args[0])] = pddl_problem.object(
                    str(k.args[1])
                )
            elif (
                fluent[0] == "calibration_target"
            ):  # get the target of each instrument
                calibration_targets[str(k.args[0])] = pddl_problem.object(
                    str(k.args[1])
                )

        for s in sat_objs:
            current_dir = satellites_at[str(s)]
            for i in ins_objs:
                # 1. switch the instrument on, otherwise continue (the instrument is not in the satellite)
                switch_on = ActionInstance(actions[1], tuple([i, s]))
                if not simulator.is_applicable(state, switch_on):
                    continue
                state = simulator.apply(state, switch_on)
                plan.append(switch_on)

                # 2. turn to calibration target to calibrate the instrument
                dest_dir = calibration_targets[str(i)]
                turn_to = ActionInstance(
                    actions[0], tuple([s, dest_dir, current_dir])
                )
                if simulator.is_applicable(state, turn_to):
                    state = simulator.apply(state, turn_to)
                    plan.append(turn_to)
                    current_dir = dest_dir

                # 3. calibrate the instrument
                calibrate = ActionInstance(
                    actions[3], tuple([s, i, current_dir])
                )
                assert simulator.is_applicable(state, calibrate)
                state = simulator.apply(state, calibrate)
                plan.append(calibrate)

                # 4. turn to each direction and take pictures in all modes
                for dir in dir_objs:
                    turn_to = ActionInstance(
                        actions[0], tuple([s, dir, current_dir])
                    )
                    if simulator.is_applicable(state, turn_to):
                        state = simulator.apply(state, turn_to)
                        current_dir = dir
                        plan.append(turn_to)
                    for m in mod_objs:
                        take_image = ActionInstance(
                            actions[4], tuple([s, current_dir, i, m])
                        )
                        if simulator.is_applicable(state, take_image):
                            state = simulator.apply(state, take_image)
                            plan.append(take_image)

                # 5. switch the instrument off
                switch_off = ActionInstance(actions[2], tuple([i, s]))
                assert simulator.is_applicable(state, switch_off)
                state = simulator.apply(state, switch_off)
                plan.append(switch_off)

            # 6. pointing to goal direction if exists
            if str(s) in g_sat.keys():
                turn_to = ActionInstance(
                    actions[0], tuple([s, g_sat[str(s)], current_dir])
                )
                if simulator.is_applicable(state, turn_to):
                    state = simulator.apply(state, turn_to)
                    plan.append(turn_to)
                    # current_dir = g_sat[str(s)]

    return SequentialPlan(plan)


def main():
    os.makedirs("training/easy/", exist_ok=True)
    os.makedirs("testing/easy/", exist_ok=True)
    os.makedirs("testing/medium/", exist_ok=True)
    os.makedirs("testing/hard/", exist_ok=True)

    reader = PDDLReader()
    """
    pddl_problem = reader.parse_problem('../../satellite/domain.pddl', '../../satellite/training/easy/p03.pddl')
    plan = generalize_plan(pddl_problem)
    print(plan)
    print(f"Is valid? {apply_plan(pddl_problem, plan)}")
    """
    all_problems = [
        f"../../satellite/training/easy/p{p:02}.pddl" for p in range(1, 100)
    ]
    all_problems.extend(
        [f"../../satellite/testing/easy/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [f"../../satellite/testing/medium/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [f"../../satellite/testing/hard/p{p:02}.pddl" for p in range(1, 31)]
    )

    all_plans = [f"training/easy/p{p:02}.plan" for p in range(1, 100)]
    all_plans.extend([f"testing/easy/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/medium/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/hard/p{p:02}.plan" for p in range(1, 31)])

    for prob, plan_file in zip(all_problems, all_plans):
        print(f"Solving {prob}...")
        pddl_problem = reader.parse_problem(
            "../../satellite/domain.pddl", prob
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
