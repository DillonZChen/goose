#!/usr/bin/env python3
import argparse
import json
import os
import subprocess
from itertools import product

CUR_DIR = os.path.dirname(os.path.realpath(__file__))
ROOT_DIR = os.path.normpath(f"{CUR_DIR}/../..")
with open(f"{ROOT_DIR}/experiments/config.json") as f:
    CONFIG = json.load(f)

DOMAINS = CONFIG["domains"]
FEATURES = CONFIG["features"]
PRUNING = CONFIG["pruning"]
OPTIMISERS = CONFIG["optimisers"]
DATA_GENERATION = CONFIG["data_generation"]
ITERATIONS = [str(i) for i in CONFIG["iterations"]]
REPEATS = [str(i) for i in range(CONFIG["repeats"])]

if os.path.exists("/pfcalcul/work/dchen"):
    CLUSTER_NAME = "pfcalcul"
    CLUSTER_TYPE = "slurm"
elif os.path.exists("/scratch/cd85/dc6693"):
    CLUSTER_NAME = "gadi"
    CLUSTER_TYPE = "pbs"
else:
    raise RuntimeError("Not on a compute cluster.")

## paths
TMP_DIR = os.path.normpath(f"{CUR_DIR}/../_tmp_train")
LCK_DIR = os.path.normpath(f"{CUR_DIR}/../_lck_train")
LOG_DIR = os.path.normpath(f"{CUR_DIR}/../_log_train/{CLUSTER_NAME}")
MDL_DIR = os.path.normpath(f"{CUR_DIR}/../_models/{CLUSTER_NAME}")
os.makedirs(LOG_DIR, exist_ok=True)
JOB_SCRIPT = f"{CUR_DIR}/job_train_{CLUSTER_TYPE}.sh"
assert os.path.exists(JOB_SCRIPT), JOB_SCRIPT

""" Main loop """


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("submissions", type=int)
    parser.add_argument("-l", "--remove_locks", action="store_true")
    parser.add_argument("-f", "--force", action="store_true")
    parser.add_argument("-w", "--which", action="store_true", help="see which jobs to go")
    parser.add_argument(
        "-rf",
        "--remove_failed",
        action="store_true",
        help="remove logs for models which failed to train or save a model",
    )
    parser.add_argument(
        "-rp",
        "--remove_pruning",
        type=str,
        help="remove all logs and models for a specific pruning",
    )
    parser.add_argument(
        "-ra",
        "--remove_all",
        action="store_true",
        help="remove everything",
    )
    args = parser.parse_args()

    submissions = args.submissions
    skipped_from_lock = 0
    skipped_from_log = 0
    to_go = 0

    if args.remove_locks:
        os.system(f"rm -rf {LCK_DIR}")
        print("Locks removed. Exiting.")
        return

    if args.remove_all:
        os.system(f"rm -rf {LCK_DIR} {LOG_DIR} {MDL_DIR} {TMP_DIR}")
        print("Everything removed. Exiting.")
        return

    submitted = 0

    for config in product(DOMAINS, FEATURES, PRUNING, OPTIMISERS, DATA_GENERATION, ITERATIONS, REPEATS):
        domain, feature, pruning, optimiser, data_gen, iterations, repeat = config
        job_description = "_".join(config)
        log_file = f"{LOG_DIR}/{job_description}.log"
        lck_file = f"{LCK_DIR}/{job_description}.lck"
        sve_file = f"{MDL_DIR}/{job_description}.model"

        os.makedirs(os.path.dirname(log_file), exist_ok=True)
        os.makedirs(os.path.dirname(lck_file), exist_ok=True)
        os.makedirs(os.path.dirname(sve_file), exist_ok=True)

        if args.remove_failed:
            if not os.path.exists(log_file):
                continue
            if os.path.exists(lck_file):
                print(f"Not removing due to lock file: {log_file}")
                continue
            with open(log_file) as f:
                content = f.read()
            if "Finished saving model" in content:
                continue
            if "Model saved to" in content:
                continue
            print(f"Removed {log_file}")
            os.remove(log_file)
            continue

        if args.remove_pruning:
            if args.remove_pruning == pruning:
                # remove log lck and sve
                for f in [log_file, lck_file, sve_file]:
                    if os.path.exists(f):
                        print(f"Removed {f}")
                        os.remove(f)
            continue

        if os.path.exists(lck_file) and not args.force:
            skipped_from_lock += 1
            continue

        if os.path.exists(log_file) and not args.force:
            skipped_from_log += 1
            continue

        if args.which:
            print(f"{job_description=}")
            to_go += 1
            continue

        if submitted >= submissions:
            to_go += 1
            continue

        data_config = os.path.normpath(f"{ROOT_DIR}/configurations/data/ipc23lt/{domain}.toml")
        assert os.path.exists(data_config), data_config

        cmd = " ".join([
            f"{ROOT_DIR}/Goose.sif",
            "train",
            data_config,
            f"--features={feature}",
            f"--pruning={pruning}",
            f"--iterations={iterations}",
            f"--optimisation={optimiser}",
            f"--data_generation={data_gen}",
            f"--random_seed={repeat}",
            f"--save_file={sve_file}",
        ])

        job_vars = ",".join([f"CMD={cmd}"])

        if CLUSTER_TYPE == "slurm":
            job_cmd = [
                "sbatch",
                f"--job-name=T_{job_description}",
                f"--output={log_file}",
                f"--export={job_vars}",
                JOB_SCRIPT,
            ]
        elif CLUSTER_TYPE == "pbs":
            job_cmd = [
                "qsub",
                "-N",
                f"T_{job_description}",
                "-o",
                log_file,
                "-j",
                "oe",
                "-v",
                job_vars,
                JOB_SCRIPT,
            ]

        with open(lck_file, "w") as f:
            f.write("")
        p = subprocess.Popen(job_cmd)
        p.wait()

        print(f" log: {log_file}")
        submitted += 1

    print(f"{submitted=}")
    print(f"{skipped_from_lock=}")
    print(f"{skipped_from_log=}")
    print(f"{to_go=}")


if __name__ == "__main__":
    main()
