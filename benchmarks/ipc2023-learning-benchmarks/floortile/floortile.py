#!/usr/bin/env python

import argparse
import os
import random
import sys
import logging
from typing import Dict


def get_tile(row: int, column: int):
    return f"tile_{row}_{column}"


def get_objects(rows: int, columns: int, robots: int, **kwargs: dict) -> str:
    offset = "\n    "

    # -- tiles
    str_objects = offset + offset.join([get_tile(r, c)
                                        for r in range(0, 1+rows)
                                        for c in range(1, 1+columns)]) + " - tile"

    # -- robots
    str_objects += offset + offset.join([f"robot{i+1}" for i in range(robots)]) + " - robot"

    # -- colors
    str_objects += offset + "white black - color\n"

    return str_objects


def get_init(rows: int, columns: int, robots: int, **kwargs: dict) -> str:
    offset = "\n    "
    str_init = ""

    # At most 1 robot per column
    assert robots <= columns
    robot_cols = [col for col in range(1, 1+columns)]
    random.shuffle(robot_cols)
    robots_at = set()
    # Robot locations and colors
    for r in range(robots):
        row = random.randint(0, rows)  # get random row
        robots_at.add((row, robot_cols[r]))
        str_init += offset + f"(robot-at robot{r+1} {get_tile(row,robot_cols[r])})"
        robot_color = random.choice(["white", "black"])
        str_init += offset + f"(robot-has robot{r+1} {robot_color})"

    # Available colors
    str_init += offset + "(available-color white)"
    str_init += offset + "(available-color black)"

    # Clear locations
    for r in range(0, 1+rows):
        for c in range(1, 1+columns):
            if (r, c) not in robots_at:
                str_init += offset + f"(clear {get_tile(r, c)})"

    # Up direction
    str_init += offset + offset.join([f"(up {get_tile(r+1,c)} {get_tile(r, c)} )"
                                      for r in range(0, rows) for c in range(1, 1+columns)])
    # Down direction
    str_init += offset + offset.join([f"(down {get_tile(r-1, c)} {get_tile(r, c)} )"
                                      for r in range(1, 1+rows) for c in range(1, 1+columns)])
    # Left direction
    str_init += offset + offset.join([f"(left {get_tile(r, c-1)} {get_tile(r, c)} )"
                                      for r in range(0, 1+rows) for c in range(2, 1+columns)])
    # Right direction
    str_init += offset + offset.join([f"(right {get_tile(r, c+1)} {get_tile(r, c)} )"
                                      for r in range(0, 1+rows) for c in range(1, columns)])

    return str_init


def get_goal(rows: int, columns: int, **kwargs: dict) -> str:
    str_goal = " (and "
    offset = "\n    "
    colors = ["white", "black"]
    str_goal += offset + offset.join([f"(painted {get_tile(r,c)} {colors[(r+c)%2]})"
                                      for r in range(1, 1+rows)
                                      for c in range(1, 1+columns)])
    return str_goal + ")"


def parse_args() -> Dict[str, int]:
    # Parser descriptor
    parser = argparse.ArgumentParser()
    parser.add_argument("-r", "--rows", type=int, help="number of rows (min 2)", required=True)
    parser.add_argument("-c", "--columns", type=int, help="number of columns (min 2)", required=True)
    parser.add_argument("-p", "--robots", type=int, help="number of robots (<=columns)", required=True)
    parser.add_argument("--seed", type=int, default=42, help="random seed (default: 42)")
    parser.add_argument("-o", "--out_folder", type=str, default=".", help="output folder (default: \".\")")
    parser.add_argument("-id", "--instance_id", type=int, default=0, help="instance id (default: 0)")

    # Parse arguments
    args = parser.parse_args()
    rows = args.rows
    columns = args.columns
    robots = args.robots
    out_f = args.out_folder
    ins_id = args.instance_id

    # Input sanity checks
    if rows < 2:
        logging.error(f" At least 2 rows required (input: rows={rows})\n")
        sys.exit(-1)
    if columns < 2:
        logging.error(f" At least 2 columns required (input: columns={columns})\n")
        sys.exit(-2)
    if robots > columns:
        logging.error(f" Robots must be less or equal to columns (input: robots={robots}; columns={columns})\n")
        sys.exit(-3)

    # Initialize data
    random.seed(args.seed)  # set the random seed here
    os.makedirs(name=out_f, exist_ok=True)  # create the output folder if that doesn't exist

    return {'rows': rows, 'columns': columns, 'robots': robots, 'out_folder': out_f, 'instance_id': ins_id, 'seed': args.seed}


def generate_problem(args: Dict):
    str_config = ', '.join([f'{k}={v}' for k, v in args.items()])
    with open(f"{args['out_folder']}/p{args['instance_id']:02}.pddl", "w") as f_problem:
        f_problem.write(f";; {str_config}\n\n"
                        f"(define (problem floortile-{args['instance_id']:02})\n"
                        f" (:domain floortile)\n"
                        f" (:objects {get_objects(**args)})\n"
                        f" (:init {get_init(**args)})\n"
                        f" (:goal {get_goal(**args)}))\n")


def main():
    args_dict = parse_args()
    generate_problem(args=args_dict)


if __name__ == "__main__":
    main()
