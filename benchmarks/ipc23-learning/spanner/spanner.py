#!/usr/bin/env python

import argparse
import os
import random
import sys
import logging
from typing import Dict


def get_objects(
    spanners: int, nuts: int, locations: int, **kwargs: dict
) -> str:
    str_objects = "\n"

    # -- man
    str_objects += "    bob - man\n    "

    # -- spanner
    str_objects += (
        " ".join([f"spanner{i+1}" for i in range(spanners)])
        + " - spanner\n    "
    )

    # -- nuts
    str_objects += (
        " ".join([f"nut{i+1}" for i in range(nuts)]) + " - nut\n    "
    )

    # -- locations
    str_objects += (
        "shed "
        + " ".join([f"location{i+1}" for i in range(locations)])
        + " gate - location\n "
    )

    return str_objects


def get_init(spanners: int, nuts: int, locations: int, **kwargs: dict) -> str:
    str_init = "\n"
    str_init += "    (at bob shed)\n"

    for i in range(spanners):
        str_init += (
            f"    (at spanner{i+1} location{random.randint(1, locations)})\n"
        )
        str_init += f"    (usable spanner{i+1})\n"

    for i in range(nuts):
        str_init += f"    (at nut{i+1} gate)\n"
        str_init += f"    (loose nut{i+1})\n"

    str_init += "    (link shed location1)\n"
    str_init += f"    (link location{locations} gate)\n"

    for i in range(locations - 1):
        str_init += f"    (link location{i+1} location{i+2})\n "

    return str_init


def get_goal(nuts: int, **kwargs: dict) -> str:
    return (
        " (and "
        + "\n   ".join([f"(tightened nut{i+1})" for i in range(nuts)])
        + ")"
    )


def parse_args() -> Dict[str, int]:
    # Parser descriptor
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-s",
        "--spanners",
        type=int,
        help="number of spanners (min 1)",
        required=True,
    )
    parser.add_argument(
        "-n",
        "--nuts",
        type=int,
        help="number of nuts (min 1 and <= spanners)",
        required=True,
    )
    parser.add_argument(
        "-l",
        "--locations",
        type=int,
        help="number of locations (min 1)",
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
    spanners = args.spanners
    nuts = args.nuts
    locations = args.locations
    out_f = args.out_folder
    ins_id = args.instance_id

    # Input sanity checks
    if spanners < 1:
        logging.error(
            f"[ERROR] At least 1 spanner required (input: spanners={spanners})\n"
        )
        sys.exit(-1)
    if nuts < 1:
        logging.error(
            f"[ERROR] At least 1 nut required (input: nuts={nuts})\n"
        )
        sys.exit(-2)
    if locations < 1:
        logging.error(
            f"[ERROR] At least 1 location required (input: locations={locations})\n"
        )
        sys.exit(-3)
    if nuts > spanners:
        logging.error(
            f"[ERROR] Nuts must be between [1,spanners] (input: nuts={nuts}; spanners={spanners})\n"
        )
        sys.exit(-4)

    # Initialize data
    random.seed(args.seed)  # set the random seed here
    os.makedirs(
        name=out_f, exist_ok=True
    )  # create the output folder if that doesn't exist

    return {
        "spanners": spanners,
        "nuts": nuts,
        "locations": locations,
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
            f"(define (problem spanner-{args['instance_id']:02})\n"
            f" (:domain spanner)\n"
            f" (:objects {get_objects(**args)})\n"
            f" (:init {get_init(**args)})\n"
            f" (:goal {get_goal(**args)}))\n"
        )


def main():
    args_dict = parse_args()
    generate_problem(args=args_dict)


if __name__ == "__main__":
    main()
