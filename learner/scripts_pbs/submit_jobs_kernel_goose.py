import os
import argparse

"""
2 SU per CPU per hour
~> 30 minute + 2 CPU job = 2 SU
"""

# 608 problems
_DOMAINS = [
    "blocks",
    "ferry",
    "gripper",
    "n-puzzle",
    "sokoban",
    "spanner",
    "visitall",
    "visitsome",
]

_REPRESENTATIONS = ["llg", "slg"]
_ITERS = [1, 3, 5]
_KERNELS = ["wl"]

_TIMEOUT = 1800

_LOG_DIR = "logs_kernel/test"
_LOCK_DIR = "lock"
_AUX_DIR = "/scratch/sv11/dc6693/aux"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-e", type=int, default=1)
    args = parser.parse_args()
    to_go = 0
    skipped = 0
    submitted = 0

    e = args.e

    def submit(domain, df, pf, model_file):
        nonlocal to_go
        nonlocal skipped
        nonlocal submitted
        nonlocal e

        if submitted >= e:
            to_go += 1
            return

        problem = os.path.basename(pf).replace(".pddl", "")

        # check whether to skip
        os.makedirs(_LOG_DIR, exist_ok=True)
        os.makedirs(_LOCK_DIR, exist_ok=True)
        os.makedirs(_AUX_DIR, exist_ok=True)
        desc = f'{domain}_{problem}_{model_file.replace("/", "-")}'
        log_file = f"{_LOG_DIR}/{desc}.log"
        lock_file = f"{_LOCK_DIR}/{desc}.lock"
        plan_file = f"{_AUX_DIR}/{desc}.plan"
        aux_file = f"{_AUX_DIR}/{desc}.aux"

        if (
            os.path.exists(log_file)
            or os.path.exists(plan_file)
            or os.path.exists(lock_file)
        ):
            skipped += 1
            return

        # submit
        with open(lock_file, "w") as f:
            pass

        cmd = (
            f"qsub -o {log_file} -j oe -v "
            + f'DOM_PATH="{df}",'
            + f'INS_PATH="{pf}",'
            + f'MODEL_PATH="{model_file}",'
            + f'TIMEOUT="{_TIMEOUT}",'
            + f'AUX_FILE="{aux_file}",'
            + f'PLAN_FILE="{plan_file}",'
            + f'LOCK_FILE="{lock_file}" '
            + f"scripts_pbs/kernel_job.sh"
        )
        os.system(cmd)
        submitted += 1
        return

    for rep in _REPRESENTATIONS:
        for iterations in _ITERS:
            for kernel in _KERNELS:
                for domain in _DOMAINS:
                    model_file = f"trained_models_kernel/{rep}_{domain}_{kernel}_{iterations}.joblib"
                    df = f"../benchmarks/goose/{domain}/domain.pddl"
                    problem_dir = f"../benchmarks/goose/{domain}/test"
                    for file in sorted(os.listdir(problem_dir)):
                        pf = f"{problem_dir}/{file}"
                        submit(domain, df, pf, model_file)
    print("submitted:", submitted)
    print("skipped:", skipped)
    print("to_go:", to_go)


if __name__ == "__main__":
    main()
