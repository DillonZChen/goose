# Import the PDDLReader and PDDLWriter classes
from unified_planning.io import PDDLReader
from unified_planning.shortcuts import SequentialSimulator
from unified_planning.plans import SequentialPlan, ActionInstance
from unified_planning.model.walkers import StateEvaluator
from queue import Queue
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
    # print(pddl_problem.user_types)  # [size, location, locatable, vehicle - locatable, package - locatable]
    utypes = pddl_problem.user_types
    actions = pddl_problem.actions  # drive, pick-up, drop
    # print(actions)

    # 0. get all vehicles, packages, and locations
    loc_objs = [o for o in pddl_problem.objects(utypes[1])]
    vehicle_objs = [o for o in pddl_problem.objects(utypes[3])]
    package_objs = [o for o in pddl_problem.objects(utypes[4])]

    # Goal fluents
    goal_fluents = [g for g in pddl_problem.goals[0].args]
    assert goal_fluents
    if goal_fluents[0].is_object_exp():
        goal_fluents = [
            g for g in pddl_problem.goals
        ]  # single fluent in the goal

    # get all are at(package,location) fluents in the goal
    g_at = {
        str(g.args[0]): pddl_problem.object(str(g.args[1]))
        for g in goal_fluents
        if str(g).split("(")[0] == "at"
    }

    with SequentialSimulator(problem=pddl_problem) as simulator:
        state = simulator.get_initial_state()
        locatable_at = dict()
        graph = dict()
        capacities = dict()
        capacity_predecessor = dict()

        # 0. get all relevant data from the instance
        for k, v in state._values.items():
            if not v.bool_constant_value():
                continue
            fluent = str(k).split("(")
            if fluent[0] == "at":  # get where the vehicle/package is at
                locatable_at[str(k.args[0])] = pddl_problem.object(
                    str(k.args[1])
                )
            elif fluent[0] == "road":  # build the underlying graph
                loc1, loc2 = pddl_problem.object(
                    str(k.args[0])
                ), pddl_problem.object(str(k.args[1]))
                if loc1 in graph.keys():
                    graph[loc1].append(loc2)
                else:
                    graph[loc1] = [loc2]
            elif fluent[0] == "capacity":  # get the car capacity
                capacities[str(k.args[0])] = pddl_problem.object(
                    str(k.args[1])
                )
            elif (
                fluent[0] == "capacity-predecessor"
            ):  # get the capacity predecessor
                capacity_predecessor[str(k.args[1])] = pddl_problem.object(
                    str(k.args[0])
                )

        def get_path(from_loc, to_loc):
            if from_loc == to_loc:
                return []
            visited_from = dict()
            for k in graph.keys():
                visited_from[k] = None
            visited_from[from_loc] = from_loc
            open_queue = Queue()
            open_queue.put(from_loc)
            while open_queue and (visited_from[to_loc] == None):
                loc = open_queue.get()
                for next_loc in graph[loc]:
                    if visited_from[next_loc] == None:
                        visited_from[next_loc] = loc
                        open_queue.put(next_loc)
            assert (
                visited_from[to_loc] != None
            )  # the graph is strongly connected, so there must be always a path
            path = []
            current_loc = to_loc
            while current_loc != from_loc:
                path.append(current_loc)
                current_loc = visited_from[current_loc]
            path = reversed(path)
            return path

        vehicle = vehicle_objs[0]  # use always the same vehicle
        vehicle_at = locatable_at[str(vehicle)]
        v_cap = capacities[str(vehicle)]
        v_cap_pre = capacity_predecessor[str(v_cap)]

        # 1. for each package that appear in the goal, drive to its origin
        for package_str, goal_package_loc in g_at.items():
            package_obj = pddl_problem.object(package_str)
            init_package_loc = locatable_at[package_str]
            path = get_path(vehicle_at, init_package_loc)
            for next_loc in path:
                drive = ActionInstance(
                    actions[0], tuple([vehicle, vehicle_at, next_loc])
                )
                assert simulator.is_applicable(state, drive)
                state = simulator.apply(state, drive)
                plan.append(drive)
                vehicle_at = next_loc

            # 2. pick-up the package in the origin
            pick_up = ActionInstance(
                actions[1],
                tuple([vehicle, vehicle_at, package_obj, v_cap_pre, v_cap]),
            )
            assert simulator.is_applicable(state, pick_up)
            state = simulator.apply(state, pick_up)
            plan.append(pick_up)

            # 3. move from package starting location, to its destination
            path = get_path(vehicle_at, goal_package_loc)
            for next_loc in path:
                drive = ActionInstance(
                    actions[0], tuple([vehicle, vehicle_at, next_loc])
                )
                assert simulator.is_applicable(state, drive)
                state = simulator.apply(state, drive)
                plan.append(drive)
                vehicle_at = next_loc

            # 4. drop the package
            drop = ActionInstance(
                actions[2],
                tuple([vehicle, vehicle_at, package_obj, v_cap_pre, v_cap]),
            )
            assert simulator.is_applicable(state, drop)
            state = simulator.apply(state, drop)
            plan.append(drop)

    return SequentialPlan(plan)


def main():
    os.makedirs("training/easy/", exist_ok=True)
    os.makedirs("testing/easy/", exist_ok=True)
    os.makedirs("testing/medium/", exist_ok=True)
    os.makedirs("testing/hard/", exist_ok=True)

    reader = PDDLReader()
    """
    pddl_problem = reader.parse_problem('../../transport/domain.pddl', '../../transport/training/easy/p50.pddl')
    plan = generalize_plan(pddl_problem)
    print(plan)
    print(f"Is valid? {apply_plan(pddl_problem, plan)}")
    """
    all_problems = [
        f"../../transport/training/easy/p{p:02}.pddl" for p in range(1, 100)
    ]
    all_problems.extend(
        [f"../../transport/testing/easy/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [f"../../transport/testing/medium/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [f"../../transport/testing/hard/p{p:02}.pddl" for p in range(1, 31)]
    )

    all_plans = [f"training/easy/p{p:02}.plan" for p in range(1, 100)]
    all_plans.extend([f"testing/easy/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/medium/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/hard/p{p:02}.plan" for p in range(1, 31)])

    for prob, plan_file in zip(all_problems, all_plans):
        print(f"Solving {prob}...")
        pddl_problem = reader.parse_problem(
            "../../transport/domain.pddl", prob
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
