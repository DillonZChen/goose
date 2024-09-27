import argparse
import os
import subprocess

from learner.model import Model


def read_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("domain_path")
    parser.add_argument("problem_path")
    parser.add_argument("model_path")
    parser.add_argument("--py", action="store_true", help="Evaluate with python code")
    parser.add_argument("--lifted", action="store_true", help="Lifted representation")
    parser.add_argument("--sas_file", type=str, default="output.sas")
    parser.add_argument("-t", "--timeout", type=int, default=1800)
    args = parser.parse_args()
    return args


def run_nfd(args, model, domain_path, problem_path, model_path, timeout):
    model_method = model.model_method

    ### NFD heuristic argument
    if args.py:
        model_method += "_py"
    heuristic = f"{model_method}(model_path={model_path},domain_path={domain_path},problem_path={problem_path})"

    ### NFD search argument
    if model.multi_heuristics:
        search_alg = "wlf_mq_eager_greedy"
    elif model.pref_schema:
        search_alg = "pref_schema_eager_greedy"
    else:
        search_alg = "batch_eager_greedy"
    search = f"{search_alg}({heuristic},max_time={timeout})"

    env = os.environ.copy()
    env["NGOOSE"] = os.getcwd()

    subprocess.run(
        [
            "python2",
            "fast-downward.py",
            "--build",
            "release64",
            "--sas_file",
            args.sas_file,
            domain_path,
            problem_path,
            "--search",
            search,
        ],
        cwd="planner/nfd",
        env=env,
    )

def run_pwl(args, model, domain_path, problem_path, model_path, timeout):
    assert not args.py, "Python code evaluation not supported with powerlifted"

    env = os.environ.copy()
    env["NGOOSE"] = os.getcwd()

    subprocess.run(
        [
            "./powerlifted.py",
            "-d",
            domain_path,
            "-i",
            problem_path,
            "--search",
            "gbfs",
            "-e",
            "wlgoose",
            "-m",
            model_path,
            "--translator-output-file",
            args.sas_file,
            "--time-limit",
            str(timeout),
        ],
        cwd="planner/pwl",
        env=env,
    )


if __name__ == "__main__":
    args = read_args()
    domain_path = os.path.abspath(args.domain_path)
    problem_path = os.path.abspath(args.problem_path)
    model_path = os.path.abspath(args.model_path)
    timeout = args.timeout

    assert os.path.exists(domain_path), domain_path
    assert os.path.exists(problem_path), problem_path
    assert os.path.exists(model_path), model_path

    model = Model.load_static(model_path)

    if args.lifted:
        run_pwl(args, model, domain_path, problem_path, model_path, timeout)
    else:
        run_nfd(args, model, domain_path, problem_path, model_path, timeout)
