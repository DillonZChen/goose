#!/usr/bin/env python

import argparse
import os
import random
import sys
import logging
from typing import Dict


def get_objects(passengers: int, floors: int, **kwargs: dict) -> str:
    str_objects = "\n    "

    # -- passengers
    str_objects += (
        " ".join([f"p{i+1}" for i in range(passengers)]) + " - passenger\n    "
    )

    # -- floors
    str_objects += (
        " ".join([f"f{i+1}" for i in range(floors)]) + " - floor\n    "
    )

    return str_objects


def get_init(passengers: int, floors: int, **kwargs: dict) -> str:
    str_init = "\n"

    # Lift starting floor
    str_init += f"    (lift-at f{random.randint(1, floors)})\n"

    # Passengers origin and destination
    for i in range(passengers):
        origin = random.randint(1, floors)
        destin = random.choice(
            [f for f in range(1, floors + 1) if f != origin]
        )  # dest != origin for each passenger
        str_init += f"    (origin p{i+1} f{origin})\n"
        str_init += f"    (destin p{i+1} f{destin})\n"

    # Second floor is above first floor
    for i in range(1, floors + 1):
        for j in range(i + 1, floors + 1):
            str_init += f"    (above f{i} f{j})\n"

    return str_init


def get_goal(passengers: int, **kwargs: dict) -> str:
    return (
        " (and "
        + "\n   ".join([f"(served p{i})" for i in range(1, passengers + 1)])
        + ")"
    )


def parse_args() -> Dict[str, int]:
    # Parser descriptor
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-p",
        "--passengers",
        type=int,
        help="number of passengers (min 1)",
        required=True,
    )
    parser.add_argument(
        "-f",
        "--floors",
        type=int,
        help="number of floors (min 2)",
        required=True,
    )
    parser.add_argument(
        "--seed", type=int, default=42, help="random seed (default: 42)"
    )
    parser.add_argument(
        "-o",
        "--out_folder",
        type=str,
        default=".",
        help='output folder (default: ".")',
    )
    parser.add_argument(
        "-i",
        "--instance_id",
        type=int,
        default=0,
        help="instance id (default: 0)",
    )

    # Parse arguments
    args = parser.parse_args()
    passengers = args.passengers
    floors = args.floors
    out_f = args.out_folder
    ins_id = args.instance_id

    # Input sanity checks
    if passengers < 1:
        logging.error(
            f"[ERROR] At least 1 passenger required (input: passengers={passengers})\n"
        )
        sys.exit(-1)
    if floors < 2:
        logging.error(
            f"[ERROR] At least 2 floors required (input: floors={floors})\n"
        )
        sys.exit(-2)

    # Initialize data
    random.seed(args.seed)  # set the random seed here
    os.makedirs(
        name=out_f, exist_ok=True
    )  # create the output folder if that doesn't exist

    return {
        "passengers": passengers,
        "floors": floors,
        "out_folder": out_f,
        "instance_id": ins_id,
        "seed": args.seed,
    }


def generate_problem(args: Dict):
    str_config = ", ".join([f"{k}={v}" for k, v in args.items()])
    with open(
        f"{args['out_folder']}/p{args['instance_id']:02}.pddl", "w"
    ) as f_problem:
        f_problem.write(
            f";; {str_config}\n\n"
            f"(define (problem miconic-{args['instance_id']:02})\n"
            f" (:domain miconic)\n"
            f" (:objects {get_objects(**args)})\n"
            f" (:init {get_init(**args)})\n"
            f" (:goal {get_goal(**args)}))\n"
        )


def main():
    args_dict = parse_args()
    generate_problem(args=args_dict)


if __name__ == "__main__":
    main()
