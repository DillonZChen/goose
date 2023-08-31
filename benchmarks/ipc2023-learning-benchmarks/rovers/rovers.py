#!/usr/bin/env python

import argparse
import os
import random
import sys
import logging
from typing import Dict, Tuple, List
from benchmarking_utils import random_connected_graph


def get_lander() -> str:
    return "general"


def get_modes() -> List[str]:
    return ["colour", "high_res", "low_res"]


def get_rover(r: int) -> str:
    return f"rover{r}"


def get_store(r: int) -> str:
    return get_rover(r) + "store"


def get_waypoint(w: int) -> str:
    return f"waypoint{w}"


def get_camera(c: int) -> str:
    return f"camera{c}"


def get_objective(o: int) -> str:
    return f"objective{o}"


def get_objects(
        rovers: int,
        waypoints: int,
        cameras: int,
        objectives: int,
        **kwargs: dict) -> str:
    offset = "\n    "
    # -- lander (just one fixed)
    str_objects = offset + f"{get_lander()} - lander"

    # -- modes (3 fixed)
    str_objects += offset + " ".join(get_modes()) + " - mode"

    # -- rovers & stores
    str_objects += offset + " ".join([get_rover(i) for i in range(1, 1+rovers)]) + " - rover"
    str_objects += offset + " ".join([get_store(i) for i in range(1, 1+rovers)]) + " - store"

    # -- waypoints
    str_objects += offset + " ".join([get_waypoint(i) for i in range(1, 1+waypoints)]) + " - waypoint"

    # -- cameras
    str_objects += offset + " ".join([get_camera(i) for i in range(1, 1+cameras)]) + " - camera"

    # -- objectives
    str_objects += offset + " ".join([get_objective(i) for i in range(1, 1+objectives)]) + " - objective"

    return str_objects


def get_init(
        rovers: int,
        waypoints: int,
        cameras: int,
        objectives: int,
        **kwargs: dict) -> Tuple[str, Tuple[List, List]]:
    offset = "\n    "
    str_init = ""

    # Lander starts in a random waypoint
    str_init += offset + f"(at_lander {get_lander()} {get_waypoint(random.randint(1,waypoints))})"

    # Each rover starts in a random waypoint
    all_rovers = [get_rover(r) for r in range(1, 1+rovers)]
    str_init += offset + offset.join([f"(at {r} {get_waypoint(random.randint(1,waypoints))})"
                                      for r in all_rovers])

    # Rover equipment: 50% chance to be in a rover; all of them supported by at least one rover
    random.shuffle(all_rovers)
    str_init += offset + offset.join([f"(equipped_for_soil_analysis {r})"
                                      for r in all_rovers[0:random.randint(1, rovers)]])
    random.shuffle(all_rovers)
    str_init += offset + offset.join([f"(equipped_for_rock_analysis {r})"
                                      for r in all_rovers[0:random.randint(1, rovers)]])
    random.shuffle(all_rovers)
    imaging_rovers = all_rovers[0:random.randint(1, rovers)]
    str_init += offset + offset.join([f"(equipped_for_imaging {r})" for r in imaging_rovers])

    # One empty store per rover
    str_init += offset + offset.join([f"(empty {get_store(r)})" for r in range(1, 1+rovers)])
    str_init += offset + offset.join([f"(store_of {get_store(r)} {get_rover(r)})"
                                      for r in range(1, 1+rovers)])

    # 50% chance of rock and/or soil samples in each waypoint
    rock_samples = [get_waypoint(w) for w in range(1, 1+waypoints) if random.choice([True, False])]
    soil_samples = [get_waypoint(w) for w in range(1, 1+waypoints) if random.choice([True, False])]
    if rock_samples:
        str_init += offset + offset.join([f"(at_rock_sample {r})" for r in rock_samples])
    if soil_samples:
        str_init += offset + offset.join([f"(at_soil_sample {s})" for s in soil_samples])

    # Random connected graph G of visible waypoints
    visible_graph, visible_tree = random_connected_graph(nodes=waypoints)
    str_init += offset + offset.join([f"(visible {get_waypoint(a)} {get_waypoint(b)})"
                                      for a, b in visible_graph])
    # Random connected graph G_r in G of can_traverse waypoints for rover r
    # (at least, G_r and G share the visible tree)
    for r in range(1, 1+rovers):
        graph_r = list(visible_tree)
        # even indexes are (a,b) and odd are (b,a)
        for idx in range(len(visible_tree), len(visible_graph), 2):
            # 30% chance of including any other edge in visible_graph
            if random.randint(1, 10) <= 3:
                graph_r.append(visible_graph[idx])
                graph_r.append(visible_graph[idx+1])
        str_init += offset + offset.join([f"(can_traverse {get_rover(r)} {get_waypoint(a)} {get_waypoint(b)})"
                                          for a, b in graph_r])

    modes_supported = set()
    for c in range(1, 1+cameras):
        # Cameras calibrate pointing to random objectives
        r_obj = random.randint(1, objectives)
        str_init += offset + f"(calibration_target {get_camera(c)} {get_objective(r_obj)})"

        # Cameras are on board random rovers that are already equipped for imaging
        r_rov = random.choice(imaging_rovers)
        str_init += offset + f"(on_board {get_camera(c)} {r_rov})"

        # Cameras have between 1 and 3 random modes
        modes = get_modes()
        random.shuffle(modes)
        num_modes = random.randint(1, len(modes))
        for m in modes[0:num_modes]:
            modes_supported.add(m)
        str_init += offset + offset.join([f"(supports {get_camera(c)} {m})" for m in modes[0:num_modes]])

    # Unsupported modes are randomly assigned to random cameras
    all_cameras = [get_camera(c) for c in range(1, 1+cameras)]
    for m in get_modes():
        if m in modes_supported:
            continue
        random.shuffle(all_cameras)
        num_cam = random.randint(1, cameras)
        str_init += offset + offset.join([f"(supports {c} {m})" for c in all_cameras[0:num_cam]])

    # Objectives visible from an arbitrary number of random waypoints (at least 1)
    all_waypoints = [get_waypoint(w) for w in range(1, 1+waypoints)]
    for o in range(1, 1+objectives):
        random.shuffle(all_waypoints)
        num_w = random.randint(1, waypoints)
        str_init += offset + offset.join([f"(visible_from {get_objective(o)} {w})"
                                          for w in all_waypoints[0:num_w]])

    return str_init, (rock_samples, soil_samples)


def get_goal(
        rock_samples: List[int],
        soil_samples: List[int],
        objectives: int,
        **kwargs: dict) -> str:
    offset = "\n    "
    str_goal = " (and "

    # Random communicated rock goals
    n_rock_goals = random.randint(0, len(rock_samples))
    random.shuffle(rock_samples)
    str_goal += offset + offset.join([f"(communicated_rock_data {r})"
                                      for r in rock_samples[0:n_rock_goals]])

    # Random communicated soil goals
    n_soil_goals = random.randint(0, len(soil_samples))
    random.shuffle(soil_samples)
    str_goal += offset + offset.join([f"(communicated_soil_data {r})"
                                      for r in soil_samples[0:n_soil_goals]])

    # Random image goals
    all_objectives_modes = [(get_objective(o), m) for o in range(1, 1+objectives) for m in get_modes()]
    random.shuffle(all_objectives_modes)
    n_image_goals = random.randint(0, len(all_objectives_modes))
    str_goal += offset + offset.join([f"(communicated_image_data {obj} {mod})"
                                      for obj, mod in all_objectives_modes[0:n_image_goals]])

    return str_goal + ")"


def parse_args() -> Dict[str, int]:
    # Parser descriptor
    parser = argparse.ArgumentParser()
    parser.add_argument("-r", "--rovers", type=int, help="number of rovers (min 1)", required=True)
    parser.add_argument("-w", "--waypoints", type=int, help="number of waypoints (min 2)", required=True)
    parser.add_argument("-c", "--cameras", type=int, help="number of cameras (min 1)", required=True)
    parser.add_argument("-o", "--objectives", type=int, help="number of objectives (min 1)", required=True)
    parser.add_argument("--seed", type=int, default=42, help="random seed (default: 42)")
    parser.add_argument("-out", "--out_folder", type=str, default=".", help="output folder (default: \".\")")
    parser.add_argument("-id", "--instance_id", type=int, default=0, help="instance id (default: 0)")

    # Parse arguments
    args = parser.parse_args()
    rovers = args.rovers
    waypoints = args.waypoints
    cameras = args.cameras
    objectives = args.objectives
    out_f = args.out_folder
    ins_id = args.instance_id

    # Input sanity checks
    if rovers < 1:
        logging.error(f" At least 1 rover required (input: rovers={rovers})\n")
        sys.exit(-1)
    if waypoints < 2:
        logging.error(f" At least 2 waypoints required (input: waypoints={waypoints})\n")
        sys.exit(-2)
    if cameras < 1:
        logging.error(f" At least 1 camera required (input: cameras={cameras})\n")
        sys.exit(-3)
    if objectives < 1:
        logging.error(f" At least 1 objective required (input: objectives={objectives})\n")
        sys.exit(-4)

    # Initialize data
    random.seed(args.seed)  # set the random seed here
    os.makedirs(name=out_f, exist_ok=True)  # create the output folder if that doesn't exist

    return {'rovers': rovers, 'waypoints': waypoints, 'cameras': cameras, 'objectives': objectives,
            'out_folder': out_f, 'instance_id': ins_id, 'seed': args.seed}


def generate_problem(args: Dict):
    str_config = ', '.join([f'{k}={v}' for k, v in args.items()])
    str_objects = get_objects(**args)
    str_init, possible_goals = get_init(**args)
    args['rock_samples'] = possible_goals[0]
    args['soil_samples'] = possible_goals[1]
    str_goal = get_goal(**args)
    with open(f"{args['out_folder']}/p{args['instance_id']:02}.pddl", "w") as f_problem:
        f_problem.write(f";; {str_config}\n\n"
                        f"(define (problem rover-{args['instance_id']:02})\n"
                        f" (:domain rover)\n"
                        f" (:objects {str_objects})\n"
                        f" (:init {str_init})\n"
                        f" (:goal {str_goal}))\n")


def main():
    args_dict = parse_args()
    generate_problem(args=args_dict)


if __name__ == "__main__":
    main()
