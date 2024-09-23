#!/usr/bin/env python

import argparse
import os
import random
import sys
import logging
from typing import Dict


def get_objects(
    children: int, trays: int, sandwiches: int, **kwargs: dict
) -> str:
    str_objects = "\n    "

    # -- children
    str_objects += (
        " ".join([f"child{i}" for i in range(1, 1 + children)])
        + " - child\n    "
    )

    # -- trays
    str_objects += (
        " ".join([f"tray{i}" for i in range(1, 1 + trays)]) + " - tray\n    "
    )

    # -- sandwiches
    str_objects += (
        " ".join([f"sandw{i}" for i in range(1, 1 + sandwiches)])
        + " - sandwich\n    "
    )

    # -- breads
    str_objects += (
        " ".join([f"bread{i}" for i in range(1, 1 + children)])
        + " - bread-portion\n    "
    )

    # -- contents
    str_objects += (
        " ".join([f"content{i}" for i in range(1, 1 + children)])
        + " - content-portion\n    "
    )

    # -- tables (fixed to 3)
    str_objects += (
        " ".join([f"table{i}" for i in range(1, 4)]) + " - place\n    "
    )

    return str_objects


def get_init(
    children: int,
    allergic_children: int,
    trays: int,
    sandwiches: int,
    **kwargs: dict,
) -> str:
    offset = "\n    "

    # All trays in the kitchen
    str_init = offset + offset.join(
        [f"(at tray{t} kitchen)" for t in range(1, 1 + trays)]
    )

    # Breads and contents at kitchen
    str_init += offset + offset.join(
        [f"(at_kitchen_bread bread{b})" for b in range(1, 1 + children)]
    )
    str_init += offset + offset.join(
        [f"(at_kitchen_content content{c})" for c in range(1, 1 + children)]
    )

    # (Non-)Allergic children
    all_children = [f"child{i}" for i in range(1, 1 + children)]
    random.shuffle(all_children)
    allergic_c = all_children[:allergic_children]
    not_allergic_c = all_children[allergic_children:]
    if allergic_children > 0:
        str_init += offset + offset.join(
            [f"(allergic_gluten {c})" for c in allergic_c]
        )
    if allergic_children < children:
        str_init += offset + offset.join(
            [f"(not_allergic_gluten {c})" for c in not_allergic_c]
        )

    # Random gluten-free bread and content
    all_bread = [f"bread{i}" for i in range(1, 1 + children)]
    random.shuffle(all_bread)
    all_content = [f"content{i}" for i in range(1, 1 + children)]
    random.shuffle(all_content)
    if allergic_children > 0:
        str_init += offset + offset.join(
            [f"(no_gluten_bread {b})" for b in all_bread[:allergic_children]]
        )
        str_init += offset + offset.join(
            [
                f"(no_gluten_content {c})"
                for c in all_content[:allergic_children]
            ]
        )

    # All children waiting for some random table
    tables = [f"table{t}" for t in range(1, 4)]
    str_init += offset + offset.join(
        [f"(waiting {c} {random.choice(tables)})" for c in all_children]
    )

    # No sandwich exists
    str_init += offset + offset.join(
        [f"(notexist sandw{s})" for s in range(1, 1 + sandwiches)]
    )

    return str_init


def get_goal(children: int, **kwargs: dict) -> str:
    return (
        " (and "
        + "\n   ".join([f"(served child{i})" for i in range(1, children + 1)])
        + ")"
    )


def parse_args() -> Dict[str, int]:
    # Parser descriptor
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-c",
        "--children",
        type=int,
        help="number of children (min 1)",
        required=True,
    )
    parser.add_argument(
        "-a",
        "--allergic",
        type=int,
        help="number of allergic children (0<=allergic<=children)",
        required=True,
    )
    parser.add_argument(
        "-t",
        "--trays",
        type=int,
        help="number of trays (min 1)",
        required=True,
    )
    parser.add_argument(
        "-s",
        "--sandwiches",
        type=int,
        help="number of sandwiches (>=children)",
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
        "-id",
        "--instance_id",
        type=int,
        default=0,
        help="instance id (default: 0)",
    )

    # Parse arguments
    args = parser.parse_args()
    children = args.children
    allergic = args.allergic
    trays = args.trays
    sandwiches = args.sandwiches
    out_f = args.out_folder
    ins_id = args.instance_id

    # Input sanity checks
    if children < 1:
        logging.error(
            f" At least 1 child required (input: children={children})\n"
        )
        sys.exit(-1)
    if allergic < 0 or allergic > children:
        logging.error(
            f" Allergic children must be >=0 and <= children (input: allergic={allergic}, children={children})\n"
        )
        sys.exit(-2)
    if trays < 1:
        logging.error(f" At least 1 tray required (input: trays={trays})\n")
        sys.exit(-3)
    if sandwiches < children:
        logging.error(
            f" Sandwiches must greater or equal to children (input: sandwiches={sandwiches}, children={children})\n"
        )
        sys.exit(-4)

    # Initialize data
    random.seed(args.seed)  # set the random seed here
    os.makedirs(
        name=out_f, exist_ok=True
    )  # create the output folder if that doesn't exist

    return {
        "children": children,
        "allergic_children": allergic,
        "trays": trays,
        "sandwiches": sandwiches,
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
            f"(define (problem childsnack-{args['instance_id']:02})\n"
            f" (:domain childsnack)\n"
            f" (:objects {get_objects(**args)})\n"
            f" (:init {get_init(**args)})\n"
            f" (:goal {get_goal(**args)}))\n"
        )


def main():
    args_dict = parse_args()
    generate_problem(args=args_dict)


if __name__ == "__main__":
    main()
