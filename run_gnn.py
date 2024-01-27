""" Main search driver. """
import argparse
import os
from planners.search import search_cmd


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "domain_pddl", type=str, help="path to domain pddl file"
    )
    parser.add_argument(
        "problem_pddl", type=str, help="path to problem pddl file"
    )
    parser.add_argument(
        "model_path",
        type=str,
        help="path to saved model weights",
    )

    parser.add_argument(
        "--algorithm",
        "-s",
        type=str,
        default="eager",
        choices=["eager"],
        help="solving algorithm using the heuristic",
    )
    parser.add_argument(
        "--aux-file",
        type=str,
        default=None,
        help="path of auxilary file such as *.sas or *.lifted",
    )
    parser.add_argument(
        "--plan-file",
        type=str,
        default=None,
        help="path of *.plan file",
    )
    args = parser.parse_args()

    cmd, aux_file = search_cmd(args, "gnn")
    print("Executing the following command:")
    print(cmd)
    print()
    os.system(cmd)
    if os.path.exists(aux_file):
        os.remove(aux_file)
