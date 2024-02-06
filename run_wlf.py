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
        "--train",
        action="store_true",
        help="perform online training (currently a work in progress)",
    )
    parser.add_argument(
        "--pybind",
        action="store_true",
        help="call python code instead of c++; useful for debugging and comparing c++ and python implemention",
    )

    parser.add_argument(
        "--algorithm",
        "-s",
        type=str,
        default="eager",
        choices=["eager", "lazy", "mq", "mqp", "lama"],
        help="solving algorithm using the heuristic",
    )
    parser.add_argument(
        "--std",
        action="store_true",
        help="compute std at initial state for bayesian models",
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

    cmd, aux_file = search_cmd(args, "wlf")
    print("Executing the following command:")
    print(cmd)
    print()
    os.system(cmd)
    if os.path.exists(aux_file):
        os.remove(aux_file)
