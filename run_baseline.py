import argparse
import os
import subprocess

ENHSP_HEURISTICS = {
    "hmrp": "sat-hmrp",
    "hmrphj": "sat-hmrphj",
    "hadd": "sat-hadd",
    "mq6": "sat-mq-3e-3eiqb2",
    "hiqb2add": "sat-hiqb2add",
}


def read_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("domain_path")
    parser.add_argument("problem_path")
    parser.add_argument(
        "baseline",
        choices=[
            "lmcut_opt",
            "lmcut_sat",
            "hmrp",
            "hmrphj",
            "hadd",
            "mq6",
            "hiqb2add",
            "mff",
        ],
    )
    parser.add_argument("--sas_file", type=str, default="output.sas")
    parser.add_argument("-t", "--timeout", type=int, default=1800)
    args = parser.parse_args()
    return args


if __name__ == "__main__":
    args = read_args()
    domain_path = os.path.abspath(args.domain_path)
    problem_path = os.path.abspath(args.problem_path)

    assert os.path.exists(domain_path), domain_path
    assert os.path.exists(problem_path), problem_path

    baseline = args.baseline

    if baseline == "lmcut_opt":
        subprocess.check_call(
            [
                "python3",
                "opt_lmcut.py",
                domain_path,
                problem_path,
                "--sas_file",
                args.sas_file,
                "--timeout",
                str(args.timeout),
            ],
            cwd="planner/nfd",
        )
    elif baseline == "lmcut_sat":
        subprocess.check_call(
            [
                "python3",
                "sat_lmcut.py",
                domain_path,
                problem_path,
                "--sas_file",
                args.sas_file,
                "--timeout",
                str(args.timeout),
            ],
            cwd="planner/nfd",
        )
    elif baseline in ENHSP_HEURISTICS:
        subprocess.check_call(
            [
                "java",
                "-jar",
                "enhsp.jar",
                "-o",
                domain_path,
                "-f",
                problem_path,
                "-planner",
                ENHSP_HEURISTICS[baseline],
                "-timeout",
                str(args.timeout),
            ],
            cwd="planner/enhsp",
        )
    elif baseline == "mff":
        subprocess.check_call(
            ["./metric_ff.sif", "-o", domain_path, "-f", problem_path, "-s", "0"],
            cwd="planner/mff",
        )
    else:
        raise NotImplementedError(f"baseline {baseline} not implemented")
