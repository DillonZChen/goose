""" Main search driver. """

import argparse
import os
from util.search import search_cmd


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("domain_pddl", type=str, help="path to domain pddl file")
    parser.add_argument("task_pddl", type=str, help="path to task pddl file")
    parser.add_argument(
        "model_type",
        type=str,
        choices=["gnn", "kernel", "linear-regression-opt", "kernel-opt"],
        help="learning model",
    )
    parser.add_argument(
        "--model-path",
        "-m",
        required=True,
        type=str,
        help="path to saved model weights",
    )
    parser.add_argument(
        "--planner",
        "-p",
        type=str,
        default="fd",
        choices=["fd", "pwl"],
        help="base c++ planner",
    )
    parser.add_argument(
        "--search",
        "-s",
        type=str,
        default="gbbfs",
        choices=["gbbfs", "gbfs"],
        help="search algorithm",
    )
    parser.add_argument(
        "--timeout", "-t", type=int, default=600, help="timeout in seconds"
    )
    parser.add_argument(
        "--aux-file",
        type=str,
        default=None,
        help="path of auxilary file such as *.sas or *.lifted",
    )
    parser.add_argument(
        "--plan-file", type=str, default=None, help="path of *.plan file"
    )
    parser.add_argument("--profile", action="store_true", help="profile with valgrind")
    args = parser.parse_args()

    cmd, intermediate_file = search_cmd(
        df=args.domain_pddl,
        pf=args.task_pddl,
        m=args.model_path,
        model_type=args.model_type,
        planner=args.planner,
        search=args.search,
        timeout=args.timeout,
        seed=0,
        aux_file=args.aux_file,
        plan_file=args.plan_file,
        profile=args.profile,
    )

    print(cmd)
    os.system(cmd)
