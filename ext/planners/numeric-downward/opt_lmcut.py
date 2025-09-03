import argparse
import os
import subprocess


def main():
    parser = argparse.ArgumentParser(description="Run LM-Cut")
    parser.add_argument("domain", help="domain file")
    parser.add_argument("problem", help="problem file")
    parser.add_argument("--plan_file", help="output plan file", default="plan")
    parser.add_argument("--sas_file", help="classical sas", default="output.sas")
    parser.add_argument("--timeout", help="timeout in seconds", type=int, default=1800)
    args = parser.parse_args()

    heuristic = "lmcutnumeric(use_second_order_simple=true,bound_iterations=10,ceiling_less_than_one=true)"
    search = f"'astar({heuristic},max_time={args.timeout})'"

    cmd = [
        "python2",
        "fast-downward.py",
        "--build",
        "release64",
        "--plan-file",
        args.plan_file,
        "--sas_file",
        args.sas_file,
        args.domain,
        args.problem,
        "--search",
        search,
    ]

    os.system(" ".join(cmd))


if __name__ == "__main__":
    main()
