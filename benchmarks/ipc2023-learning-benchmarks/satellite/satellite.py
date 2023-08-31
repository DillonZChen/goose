#!/usr/bin/env python

import argparse
import os
import random
import sys
import logging
from typing import Dict


def get_objects(satellites: int, instruments: int, modes: int, directions: int, **kwargs: dict) -> str:
    str_objects = "\n    "

    # -- satellites
    str_objects += " ".join([f"sat{i}" for i in range(1, 1+satellites)]) + " - satellite\n    "

    # -- instruments
    str_objects += " ".join([f"ins{i}" for i in range(1, 1+instruments)]) + " - instrument\n    "

    # -- modes
    str_objects += " ".join([f"mod{i}" for i in range(1, 1+modes)]) + " - mode\n    "

    # -- directions
    str_objects += " ".join([f"dir{i}" for i in range(1, 1+directions)]) + " - direction\n    "

    return str_objects


def get_init(satellites: int, instruments: int, modes: int, directions: int, **kwargs: dict) -> str:
    offset = "\n    "

    # 1. Satellites point to random locations
    str_init = offset + offset.join([f"(pointing sat{s} dir{random.randint(1,directions)})"
                                     for s in range(1, 1+satellites)])
    # 2. All satellites have power available
    str_init += offset + offset.join([f"(power_avail sat{s})" for s in range(1, 1+satellites)])

    # 3. Instruments have random calibration targets
    str_init += offset + offset.join([f"(calibration_target ins{i} dir{random.randint(1,directions)})"
                                      for i in range(1, 1+instruments)])
    # 4.a. one random instrument per satellite (instruments >= satellites)
    assert instruments >= satellites
    satellite_ids = [s for s in range(1, 1+satellites)]
    random.shuffle(satellite_ids)
    onboard_instruments = [f"(on_board ins{s} sat{satellite_ids[s-1]})" for s in range(1, 1+satellites)]
    # 4.b. the rest of instruments randomly assigned
    onboard_instruments.extend([f"(on_board ins{i} sat{random.randint(1, satellites)})"
                                for i in range(1+satellites, 1+instruments)])
    str_init += offset + offset.join(onboard_instruments)
    # 5. keep adding what each instrument support until all instruments have at
    #    least one mode, and each mode is at least assigned to an instrument
    avail_instruments = [True] * instruments
    avail_modes = [True] * modes
    total_instruments, total_modes = instruments, modes
    support_combs = [(i, m) for i in range(instruments) for m in range(modes)]
    random.shuffle(support_combs)
    current_comb = 0
    while total_instruments or total_modes:
        inst, mode = support_combs[current_comb]
        current_comb += 1
        if avail_instruments[inst]:
            avail_instruments[inst] = False
            total_instruments -= 1
        if avail_modes[mode]:
            avail_modes[mode] = False
            total_modes -= 1
        str_init += offset + f"(supports ins{inst+1} mod{mode+1})"

    return str_init


def get_goal(satellites: int, directions: int, modes: int, **kwargs: dict) -> str:
    str_goal = " (and "

    # Random goal pointing directions to 50% of satellites
    str_goal += "\n   ".join([f"(pointing sat{s} dir{random.randint(1, directions)})"
                              for s in range(1, 1+satellites)
                              if random.choice([True, False])])

    # Images from random directions and modes
    observation_combs = [(d+1, m+1) for d in range(directions) for m in range(modes)]
    random.shuffle(observation_combs)
    rand_num_obs = random.randint(1, len(observation_combs))
    for obs_id in range(rand_num_obs):
        d, m = observation_combs[obs_id]
        str_goal += "\n   " + f"(have_image dir{d} mod{m})"

    return str_goal + ")"


def parse_args() -> Dict[str, int]:
    # Parser descriptor
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--satellites", type=int, help="number of satellites (min 1)", required=True)
    parser.add_argument("-i", "--instruments", type=int, help="number of instruments (min 1)", required=True)
    parser.add_argument("-m", "--modes", type=int, help="number of modes (min 1)", required=True)
    parser.add_argument("-d", "--directions", type=int, help="number of directions (min 2)", required=True)
    parser.add_argument("--seed", type=int, default=42, help="random seed (default: 42)")
    parser.add_argument("-o", "--out_folder", type=str, default=".", help="output folder (default: \".\")")
    parser.add_argument("-id", "--instance_id", type=int, default=0, help="instance id (default: 0)")

    # Parse arguments
    args = parser.parse_args()
    satellites = args.satellites
    instruments = args.instruments
    modes = args.modes
    directions = args.directions
    out_f = args.out_folder
    ins_id = args.instance_id

    # Input sanity checks
    if satellites < 1:
        logging.error(f" At least 1 satellite required (input: satellites={satellites})\n")
        sys.exit(-1)
    if instruments < 1:
        logging.error(f" At least 1 instrument required (input: instruments={instruments})\n")
        sys.exit(-2)
    if modes < 1:
        logging.error(f" At least 1 mode required (input: modes={modes})\n")
        sys.exit(-3)
    if directions < 2:
        logging.error(f" At least 2 directions required (input: directions={directions})\n")
        sys.exit(-4)

    # Initialize data
    random.seed(args.seed)  # set the random seed here
    os.makedirs(name=out_f, exist_ok=True)  # create the output folder if that doesn't exist

    return {'satellites': satellites, 'instruments': instruments,
            'modes': modes, 'directions': directions,
            'out_folder': out_f, 'instance_id': ins_id, 'seed': args.seed}


def generate_problem(args: Dict):
    str_config = ', '.join([f'{k}={v}' for k, v in args.items()])
    with open(f"{args['out_folder']}/p{args['instance_id']:02}.pddl", "w") as f_problem:
        f_problem.write(f";; {str_config}\n\n"
                        f"(define (problem satellite-{args['instance_id']:02})\n"
                        f" (:domain satellite)\n"
                        f" (:objects {get_objects(**args)})\n"
                        f" (:init {get_init(**args)})\n"
                        f" (:goal {get_goal(**args)}))\n")


def main():
    args_dict = parse_args()
    generate_problem(args=args_dict)


if __name__ == "__main__":
    main()
