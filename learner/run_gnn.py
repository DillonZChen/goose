""" Main search driver. """

import argparse
import os
from util.search import search_cmd


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("domain_pddl", type=str, help="path to domain pddl file")
    parser.add_argument("task_pddl", type=str, help="path to task pddl file")
    parser.add_argument("model_path", type=str, help="path to trained and saved model weights")
    parser.add_argument(
        "--planner",
        "-p",
        type=str,
        default="fd",
        choices=["fd", "pwl"],
        help="backend search implementation",
    )
    parser.add_argument(
        "--search",
        "-s",
        type=str,
        default="gbbfs",
        choices=["gbbfs", "gbfs"],
        help="search algorithm",
    )
    parser.add_argument("--timeout", "-t", type=int, default=630, help="timeout in seconds")
    args = parser.parse_args()

    cmd, intermediate_file = search_cmd(args)

    print(cmd)
    os.system(cmd)

    if os.path.exists(intermediate_file):
        os.remove(intermediate_file)
