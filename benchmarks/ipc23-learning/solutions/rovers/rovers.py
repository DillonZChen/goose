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
    # print(pddl_problem.user_types)  # [rover, waypoint, store, camera, mode, lander, objective]
    utypes = pddl_problem.user_types
    # navigate, sample_soil, sample_rock, drop, calibrate, take_image, communicate_soil_data, communicate_rock_data, communicate_image_data
    actions = pddl_problem.actions
    # print(actions)

    def get_obj(name):
        return pddl_problem.object(str(name))

    # Goal fluents
    goal_fluents = [g for g in pddl_problem.goals[0].args]
    assert goal_fluents
    if goal_fluents[0].is_object_exp():
        goal_fluents = [
            g for g in pddl_problem.goals
        ]  # single fluent in the goal

    # print(goal_fluents)

    g_soil = [
        get_obj(g.args[0])
        for g in goal_fluents
        if str(g).split("(")[0] == "communicated_soil_data"
    ]
    g_rock = [
        get_obj(g.args[0])
        for g in goal_fluents
        if str(g).split("(")[0] == "communicated_rock_data"
    ]
    g_images = [
        (get_obj(g.args[0]), get_obj(g.args[1]))
        for g in goal_fluents
        if str(g).split("(")[0] == "communicated_image_data"
    ]
    # print(g_soil)
    # print(g_rock)
    # print(g_images)

    # 0. get all objects
    rov_objs = [o for o in pddl_problem.objects(utypes[0])]
    way_objs = [o for o in pddl_problem.objects(utypes[1])]
    store_objs = [o for o in pddl_problem.objects(utypes[2])]
    cam_objs = [o for o in pddl_problem.objects(utypes[3])]
    mod_objs = [o for o in pddl_problem.objects(utypes[4])]
    lander_objs = [o for o in pddl_problem.objects(utypes[5])]
    objective_objs = [o for o in pddl_problem.objects(utypes[6])]

    with SequentialSimulator(problem=pddl_problem) as simulator:
        state = simulator.get_initial_state()
        graph = {
            r: dict() for r in rov_objs
        }  # can_traverse: waypoint -> waypoint for each rover
        visible = dict()  # waypoint -> waypoint
        visible_from = dict()  # objective -> waypoints
        supports = dict()  # mode -> cameras
        # on_board = dict()  # camera -> rover
        on_board = {r: set() for r in rov_objs}  # rover -> cameras
        store = dict()  # rover -> store
        calibration_target = dict()  # camera -> objective
        at = dict()  # rover -> waypoint
        at_lander = (
            None  # waypoint (goal, communicate everything to the lander)
        )
        soil_rovers = []
        rock_rovers = []
        image_rovers = []

        # 0.a get all relevant data from the instance
        for k, v in state._values.items():
            if not v.bool_constant_value():
                continue
            fluent = str(k).split("(")
            if fluent[0] == "at":  # get where the rover is at
                at[str(k.args[0])] = get_obj(k.args[1])
            elif (
                fluent[0] == "can_traverse"
            ):  # build the underlying rover graph
                r, w1, w2 = (
                    get_obj(k.args[0]),
                    get_obj(k.args[1]),
                    get_obj(k.args[2]),
                )
                if w1 in graph[r].keys():
                    graph[r][w1].add(w2)
                else:
                    graph[r][w1] = set([w2])
            elif fluent[0] == "visible":  # build the underlying visible graph
                w1, w2 = get_obj(k.args[0]), get_obj(k.args[1])
                if w1 in visible.keys():
                    visible[w1].add(w2)
                else:
                    visible[w1] = set([w2])
            elif fluent[0] == "visible_from":
                o, w = str(k.args[0]), get_obj(k.args[1])
                if o in visible_from.keys():
                    visible_from[o].add(w)
                else:
                    visible_from[o] = set([w])
            elif fluent[0] == "at_lander":
                at_lander = get_obj(
                    k.args[1]
                )  # there is always one single lander
            elif fluent[0] == "store_of":  # get the store of a rover
                store[str(k.args[1])] = get_obj(k.args[0])
            elif (
                fluent[0] == "calibration_target"
            ):  # get the calibration objective of a camera
                calibration_target[str(k.args[0])] = get_obj(k.args[1])
            elif fluent[0] == "on_board":
                # on_board[str(k.args[0])] = get_obj(k.args[1])
                camera, rover = get_obj(k.args[0]), get_obj(k.args[1])
                if rover in on_board.keys():
                    on_board[rover].add(camera)
                else:
                    on_board[rover] = set([camera])
            elif fluent[0] == "supports":
                c, m = get_obj(k.args[0]), str(k.args[1])
                if m in supports.keys():
                    supports[m].add(c)
                else:
                    supports[m] = set([c])
            elif fluent[0] == "equipped_for_soil_analysis":
                soil_rovers.append(get_obj(k.args[0]))
            elif fluent[0] == "equipped_for_rock_analysis":
                rock_rovers.append(get_obj(k.args[0]))
            elif fluent[0] == "equipped_for_imaging":
                image_rovers.append(get_obj(k.args[0]))

        # 0.b get a path from rover's location to a goal set of waypoint
        def get_path(rover, waypoints):
            rover_at = at[str(rover)]
            if rover_at in waypoints:
                return [rover_at]

            g = graph[rover]
            visited_from = {w: None for w in way_objs}
            visited_from[rover_at] = rover_at
            queue = Queue()
            queue.put(rover_at)

            while queue:
                current_at = queue.get()
                for next_w in g[current_at]:
                    # Skip if it is not visible
                    if not (next_w in visible[current_at]):
                        continue

                    # Expand to next waypoints
                    if visited_from[next_w] == None:
                        visited_from[next_w] = current_at
                        queue.put(next_w)

                    # Goal achieved!
                    if next_w in waypoints:
                        # Recover path to goal
                        path = []
                        while visited_from[next_w] != next_w:
                            path.append(next_w)
                            next_w = visited_from[next_w]
                        path.append(rover_at)

                        return [w for w in reversed(path)]

            return None

        # 1.b select a rover that has a path to soil sample, then to lander, and
        #     that is equipped for soil analysis
        def solve_soil(waypoint):
            for r in soil_rovers:
                path = get_path(r, set([waypoint]))
                if path is None:
                    continue
                at[str(r)] = path[-1]
                # move to a visible adjacent waypoint of lander
                path2 = get_path(r, visible[at_lander])
                if path2 is None:
                    at[str(r)] = path[0]
                    continue
                # print(f"Rover {r} => {path} => {path2}")
                local_plan = []

                # 1. traverse 1
                if len(path) > 1:
                    for c_way, n_way in zip(path[:-1], path[1:]):
                        navigate = ActionInstance(
                            actions[0], tuple([r, c_way, n_way])
                        )
                        # assert simulator.is_applicable(state, navigate)
                        # state = simulator.apply(state, navigate)
                        local_plan.append(navigate)

                # 2. sample soil
                sample_soil = ActionInstance(
                    actions[1], tuple([r, store[str(r)], path[-1]])
                )
                # assert simulator.is_applicable(state, sample_soil)
                # state = simulator.apply(state, sample_soil)
                local_plan.append(sample_soil)

                # 3. drop
                drop = ActionInstance(actions[3], tuple([r, store[str(r)]]))
                # assert simulator.is_applicable(state, drop)
                # state = simulator.apply(state, drop)
                local_plan.append(drop)

                # 4. traverse 2
                if len(path2) > 1:
                    for c_way, n_way in zip(path2[:-1], path2[1:]):
                        navigate = ActionInstance(
                            actions[0], tuple([r, c_way, n_way])
                        )
                        # assert simulator.is_applicable(state, navigate)
                        # state = simulator.apply(state, navigate)
                        local_plan.append(navigate)

                at[str(r)] = path2[-1]

                # 5. communicate soil
                communicate_soil = ActionInstance(
                    actions[6],
                    tuple(
                        [r, lander_objs[0], waypoint, at[str(r)], at_lander]
                    ),
                )
                # assert simulator.is_applicable(state, communicate_soil)
                # state = simulator.apply(state, communicate_soil)
                local_plan.append(communicate_soil)

                return local_plan

        # 1.a Solve each sample soil subproblem independently
        for soil_waypoint in g_soil:
            communicate_soil_plan = solve_soil(soil_waypoint)
            for act in communicate_soil_plan:
                # print(act)
                assert simulator.is_applicable(state, act)
                state = simulator.apply(state, act)
                plan.append(act)

        # 2.b select a rover that has a path to rock sample, then to lander, and
        #     that is equipped for rock analysis
        def solve_rock(waypoint):
            for r in rock_rovers:
                path = get_path(r, set([waypoint]))
                if path is None:
                    continue
                at[str(r)] = path[-1]
                # move to a visible adjacent waypoint of lander
                path2 = get_path(r, visible[at_lander])
                if path2 is None:
                    at[str(r)] = path[0]
                    continue
                # print(f"Rover {r} => {path} => {path2}")
                local_plan = []

                # 1. traverse 1
                if len(path) > 1:
                    for c_way, n_way in zip(path[:-1], path[1:]):
                        navigate = ActionInstance(
                            actions[0], tuple([r, c_way, n_way])
                        )
                        # assert simulator.is_applicable(state, navigate)
                        # state = simulator.apply(state, navigate)
                        local_plan.append(navigate)

                # 2. sample soil
                sample_rock = ActionInstance(
                    actions[2], tuple([r, store[str(r)], path[-1]])
                )
                # assert simulator.is_applicable(state, sample_soil)
                # state = simulator.apply(state, sample_soil)
                local_plan.append(sample_rock)

                # 3. drop
                drop = ActionInstance(actions[3], tuple([r, store[str(r)]]))
                # assert simulator.is_applicable(state, drop)
                # state = simulator.apply(state, drop)
                local_plan.append(drop)

                # 4. traverse 2
                if len(path2) > 1:
                    for c_way, n_way in zip(path2[:-1], path2[1:]):
                        navigate = ActionInstance(
                            actions[0], tuple([r, c_way, n_way])
                        )
                        # assert simulator.is_applicable(state, navigate)
                        # state = simulator.apply(state, navigate)
                        local_plan.append(navigate)

                at[str(r)] = path2[-1]

                # 5. communicate rock
                communicate_rock = ActionInstance(
                    actions[7],
                    tuple(
                        [r, lander_objs[0], waypoint, at[str(r)], at_lander]
                    ),
                )
                # assert simulator.is_applicable(state, communicate_rock)
                # state = simulator.apply(state, communicate_rock)
                local_plan.append(communicate_rock)

                return local_plan

        # 2.a Solve each sample rock subproblem independently
        for rock_waypoint in g_rock:
            communicate_rock_plan = solve_rock(rock_waypoint)
            for act in communicate_rock_plan:
                # print(act)
                assert simulator.is_applicable(state, act)
                state = simulator.apply(state, act)
                plan.append(act)

        # 3.c calibrate rover camera
        def calibrate_camera(rover, camera):
            # 3.a.1 move next to calibration target
            objective = calibration_target[str(camera)]
            waypoints = visible_from[str(objective)]
            path = get_path(rover, waypoints)
            local_plan = []
            if len(path) > 1:
                for c_way, n_way in zip(path[:-1], path[1:]):
                    navigate = ActionInstance(
                        actions[0], tuple([rover, c_way, n_way])
                    )
                    # assert simulator.is_applicable(state, navigate)
                    # state = simulator.apply(state, navigate)
                    local_plan.append(navigate)

            at[str(rover)] = path[-1]

            calibrate = ActionInstance(
                actions[4], tuple([rover, camera, objective, at[str(rover)]])
            )
            # assert simulator.is_applicable(state, calibrate)
            # state = simulator.apply(state, calibrate)
            local_plan.append(calibrate)

            return local_plan

        # 3.b select a rover that has a path to take the image, then to lander, and
        #     that is equipped for imaging
        def solve_image(objective, mode):
            waypoints = visible_from[str(objective)]
            cameras = supports[str(mode)]
            for r in image_rovers:
                # 0.a Select a camera that supports the right mode
                camera = None
                for c in on_board[r]:
                    if c in cameras:
                        camera = c
                        break
                if camera == None:
                    continue

                # 0.b calibrate the camera
                original_rover_at = at[str(r)]
                local_plan = calibrate_camera(r, camera)

                # 0.c get paths from rover to objective, and then to lander
                path = get_path(r, waypoints)
                if path is None:
                    at[str(r)] = original_rover_at
                    continue
                at[str(r)] = path[-1]
                # move to a visible adjacent waypoint of lander
                path2 = get_path(r, visible[at_lander])
                if path2 is None:
                    at[str(r)] = original_rover_at
                    continue

                # 1. traverse 1
                if len(path) > 1:
                    for c_way, n_way in zip(path[:-1], path[1:]):
                        navigate = ActionInstance(
                            actions[0], tuple([r, c_way, n_way])
                        )
                        # assert simulator.is_applicable(state, navigate)
                        # state = simulator.apply(state, navigate)
                        local_plan.append(navigate)

                # 2. take image
                take_image = ActionInstance(
                    actions[5], tuple([r, path[-1], objective, camera, mode])
                )
                # assert simulator.is_applicable(state, take_image)
                # state = simulator.apply(state, take_image)
                local_plan.append(take_image)

                # 3. traverse 2
                if len(path2) > 1:
                    for c_way, n_way in zip(path2[:-1], path2[1:]):
                        navigate = ActionInstance(
                            actions[0], tuple([r, c_way, n_way])
                        )
                        # assert simulator.is_applicable(state, navigate)
                        # state = simulator.apply(state, navigate)
                        local_plan.append(navigate)

                at[str(r)] = path2[-1]

                # 4. communicate image
                communicate_image = ActionInstance(
                    actions[8],
                    tuple(
                        [
                            r,
                            lander_objs[0],
                            objective,
                            mode,
                            at[str(r)],
                            at_lander,
                        ]
                    ),
                )
                # assert simulator.is_applicable(state, communicate_image)
                # state = simulator.apply(state, communicate_image)
                local_plan.append(communicate_image)

                return local_plan

        # 3.a Solve each image subproblem independently
        if g_images:
            # print(plan)
            # For each goal objective and mode, get a plan
            for objective, mode in g_images:
                take_image_plan = solve_image(objective, mode)
                for act in take_image_plan:
                    # print(act)
                    assert simulator.is_applicable(state, act)
                    state = simulator.apply(state, act)
                    plan.append(act)

    return SequentialPlan(plan)


def main():
    os.makedirs("training/easy/", exist_ok=True)
    os.makedirs("testing/easy/", exist_ok=True)
    os.makedirs("testing/medium/", exist_ok=True)
    os.makedirs("testing/hard/", exist_ok=True)

    reader = PDDLReader()
    """
    pddl_problem = reader.parse_problem('../../rovers/domain.pddl', '../../rovers/training/easy/p06.pddl')
    plan = generalize_plan(pddl_problem)
    print(plan)
    print(f"Is valid? {apply_plan(pddl_problem, plan)}")
    """
    all_problems = [
        f"../../rovers/training/easy/p{p:02}.pddl" for p in range(1, 100)
    ]
    all_problems.extend(
        [f"../../rovers/testing/easy/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [f"../../rovers/testing/medium/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [f"../../rovers/testing/hard/p{p:02}.pddl" for p in range(1, 31)]
    )

    all_plans = [f"training/easy/p{p:02}.plan" for p in range(1, 100)]
    all_plans.extend([f"testing/easy/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/medium/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/hard/p{p:02}.plan" for p in range(1, 31)])

    for prob, plan_file in zip(all_problems, all_plans):
        print(f"Solving {prob}...")
        pddl_problem = reader.parse_problem("../../rovers/domain.pddl", prob)
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
