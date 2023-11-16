#!/usr/bin/env python

import argparse
import os
import random
import sys
import logging
from typing import Dict, List, Tuple


def get_objects(cars: int, locations: int, **kwargs: dict) -> str:
    str_objects = "\n    "

    # -- cars
    str_objects += " ".join([f"car{i}" for i in range(1, 1+cars)]) + " - car\n    "

    # -- locations
    str_objects += " ".join([f"loc{i+1}" for i in range(locations)]) + " - location\n "

    return str_objects


def get_init(cars: int, locations: int, **kwargs: dict) -> Tuple[str, List[int]]:
    str_init = "\n"
    # The ferry starts empty and in a random location
    str_init += f"    (empty-ferry)\n"
    str_init += f"    (at-ferry loc{random.randint(1, locations)})\n"

    # Starting cars locations
    origin_car_locations = [random.randint(1, locations) for _ in range(cars)]
    for i in range(1, 1+cars):
        str_init += f"    (at car{i} loc{origin_car_locations[i-1]})\n"

    return str_init, origin_car_locations


def get_goal(cars: int, locations: int, origin_car_locations: List[int], **kwargs: dict) -> str:
    goal_car_locations = []
    for i in range(cars):
        goal_car_locations.append(
            random.choice([dest for dest in range(1, 1+locations) if dest != origin_car_locations[i]]))

    return " (and " + "\n   ".join([f"(at car{i} loc{goal_car_locations[i-1]})" for i in range(1, 1+cars)]) + ")"


def parse_args() -> Dict[str, int]:
    # Parser descriptor
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--cars", type=int, help="number of cars (min 1)", required=True)
    parser.add_argument("-l", "--locations", type=int, help="number of locations (min 2)", required=True)
    parser.add_argument("--seed", type=int, default=42, help="random seed (default: 42")
    parser.add_argument("-o", "--out_folder", type=str, default=".", help="output folder (default: \".\")")
    parser.add_argument("-i", "--instance_id", type=int, default=0, help="instance id (default: 0)")

    # Parse arguments
    args = parser.parse_args()
    cars = args.cars
    locations = args.locations
    out_f = args.out_folder
    ins_id = args.instance_id

    # Input sanity checks
    if cars < 1:
        logging.error(f"[ERROR] At least 1 car required (input: cars={cars})\n")
        sys.exit(-1)
    if locations < 2:
        logging.error(f"[ERROR] At least 2 locations required (input: locations={locations})\n")
        sys.exit(-2)

    # Initialize data
    random.seed(args.seed)  # set the random seed here
    os.makedirs(name=out_f, exist_ok=True)  # create the output folder if that doesn't exist

    return {'cars': cars, 'locations': locations, 'out_folder': out_f, 'instance_id': ins_id, 'seed': args.seed}


def generate_problem(args: Dict):
    str_config = ', '.join([f'{k}={v}' for k, v in args.items()])
    with open(f"{args['out_folder']}/p{args['instance_id']:02}.pddl", "w") as f_problem:
        str_objects = get_objects(**args)
        str_init, args['origin_car_locations'] = get_init(**args)
        str_goal = get_goal(**args)
        f_problem.write(f";; {str_config}\n\n"
                        f"(define (problem ferry-{args['instance_id']:02})\n"
                        f" (:domain ferry)\n"
                        f" (:objects {str_objects})\n"
                        f" (:init {str_init})\n"
                        f" (:goal {str_goal}))\n")


def main():
    args_dict = parse_args()
    generate_problem(args=args_dict)


if __name__ == "__main__":
    main()
