import argparse
import os
import subprocess


def read_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("domain_path")
    parser.add_argument("problem_path")
    parser.add_argument("plan_path")
    args = parser.parse_args()
    return args


if __name__ == "__main__":
    args = read_args()
    domain_path = os.path.abspath(args.domain_path)
    problem_path = os.path.abspath(args.problem_path)
    plan_path = os.path.abspath(args.plan_path)

    assert os.path.exists(domain_path), domain_path
    assert os.path.exists(problem_path), problem_path
    assert os.path.exists(plan_path), plan_path

    subprocess.check_call(
        [
            "python2",
            "fast-downward.py",
            "--build",
            "release64",
            "--sas_file",
            "sas_file.sas",
            domain_path,
            problem_path,
            "--search",
            f"plan_trace_successors(plan_path={plan_path},validate_only=true)",
        ]
    )
