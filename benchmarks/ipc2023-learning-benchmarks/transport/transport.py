#!/usr/bin/env python

import argparse
import os
import random
import sys
import logging
from typing import Dict, List
from benchmarking_utils import random_connected_graph


def get_objects(vehicles: int, packages: int, locations: int, max_capacity: int, **kwargs: dict) -> str:
    str_objects = "\n    "

    # -- vehicles
    str_objects += " ".join([f"v{i}" for i in range(1, 1 + vehicles)]) + " - vehicle\n    "

    # -- package
    str_objects += " ".join([f"p{i}" for i in range(1, 1 + packages)]) + " - package\n    "

    # -- locations
    str_objects += " ".join([f"l{i}" for i in range(1, 1 + locations)]) + " - location\n    "

    # -- sizes
    str_objects += " ".join([f"c{i}" for i in range(0, 1 + max_capacity)]) + " - size\n    "
    return str_objects


def get_init(vehicles: int, packages: int, locations: int, max_capacity: int, **kwargs: dict) -> str:
    # All vehicles have random capacity between [1,max_capacity]
    offset = "\n    "
    str_init = offset.join([f"(capacity v{i} c{random.randint(1, max_capacity)})"
                            for i in range(1, 1 + vehicles)]) + offset

    # Create capacity relations
    str_init += offset.join([f"(capacity-predecessor c{i} c{i + 1})"
                             for i in range(max_capacity)]) + offset

    # Packages are in random locations
    origins = [random.randint(1, locations) for _ in range(packages)]
    str_init += offset.join([f"(at p{i} l{origins[i - 1]})"
                             for i in range(1, 1 + packages)]) + offset

    # Vehicles are in random locations
    str_init += offset.join([f"(at v{i} l{random.randint(1, locations)})"
                             for i in range(1, 1 + vehicles)]) + offset

    # Generate random graph
    graph, _ = random_connected_graph(nodes=locations)
    str_init += offset.join([f"(road l{l1} l{l2})" for l1, l2 in graph]) + offset

    return str_init, origins


def get_goal(packages: int, locations: int, origins: List[int], **kwargs: dict) -> str:
    offset = "\n    "
    str_goal = " (and "
    for p in range(1, 1+packages):
        dest = origins[p-1]
        while dest == origins[p-1]:
            dest = random.randint(1, locations)
        str_goal += offset + f"(at p{p} l{dest})"
    str_goal += ")"
    return str_goal


def parse_args() -> Dict[str, int]:
    # Parser descriptor
    parser = argparse.ArgumentParser()
    parser.add_argument("-v", "--vehicles", type=int, help="number of vehicles (min 1)", required=True)
    parser.add_argument("-p", "--packages", type=int, help="number of packages (min 1)", required=True)
    parser.add_argument("-l", "--locations", type=int, help="number of locations (min 2)", required=True)
    parser.add_argument("-m", "--max_capacity", type=int, help="number of packages (min 1)", required=True)
    parser.add_argument("--seed", type=int, default=42, help="random seed (default: 42)")
    parser.add_argument("-o", "--out_folder", type=str, default=".", help="output folder (default: \".\")")
    parser.add_argument("-i", "--instance_id", type=int, default=0, help="instance id (default: 0)")

    # Parse arguments
    args = parser.parse_args()
    vehicles = args.vehicles
    packages = args.packages
    locations = args.locations
    max_capacity = args.max_capacity
    out_f = args.out_folder
    ins_id = args.instance_id

    # Input sanity checks
    if vehicles < 1:
        logging.error(f" At least 1 passenger required (input: vehicles={vehicles})\n")
        sys.exit(-1)
    if packages < 1:
        logging.error(f" At least 1 package required (input: packages={packages})\n")
        sys.exit(-2)
    if locations < 2:
        logging.error(f" At least 2 locations required (input: locations={locations})\n")
        sys.exit(-3)
    if max_capacity < 1:
        logging.error(f" At least max_capacity=1 required (input: max_capacity={max_capacity})\n")
        sys.exit(-4)

    # Initialize data
    random.seed(args.seed)  # set the random seed here
    os.makedirs(name=out_f, exist_ok=True)  # create the output folder if that doesn't exist

    return {'vehicles': vehicles, 'packages': packages,
            'locations': locations, 'max_capacity': max_capacity,
            'out_folder': out_f, 'instance_id': ins_id, 'seed': args.seed}


def generate_problem(args: Dict):
    str_config = ', '.join([f'{k}={v}' for k, v in args.items()])
    with open(f"{args['out_folder']}/p{args['instance_id']:02}.pddl", "w") as f_problem:
        str_objects = get_objects(**args)
        str_init, args['origins'] = get_init(**args)
        str_goal = get_goal(**args)
        f_problem.write(f";; {str_config}\n\n"
                        f"(define (problem transport-{args['instance_id']:02})\n"
                        f" (:domain transport)\n"
                        f" (:objects {str_objects})\n"
                        f" (:init {str_init})\n"
                        f" (:goal {str_goal}))\n")


def main():
    args_dict = parse_args()
    generate_problem(args=args_dict)


if __name__ == "__main__":
    main()
