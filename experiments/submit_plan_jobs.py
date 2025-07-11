#!/usr/bin/env python3

import argparse
import os
import subprocess

from fixtures import Perc, get_configs, get_model_config_combinations

CONFIGS = get_configs("plan")

CUR_DIR = CONFIGS["CUR_DIR"]
GOOSE_DIR = CONFIGS["GOOSE_DIR"]
GOOSE_SIF = CONFIGS["GOOSE_SIF"]
TMP_DIR = CONFIGS["TMP_DIR"]
LOG_DIR = CONFIGS["LOG_DIR"]
LOCK_DIR = CONFIGS["LOCK_DIR"]
MODEL_DIR = CONFIGS["MODEL_DIR"]
SAS_BENCHMARK_DIR = CONFIGS["SAS_BENCHMARK_DIR"]
PDDL_BENCHMARK_DIR = CONFIGS["PDDL_BENCHMARK_DIR"]
JOB_SCRIPT = CONFIGS["JOB_SCRIPT"]

""" Main loop """


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("submissions", nargs="?", default=0, type=int)
    parser.add_argument("-l", "--remove_locks", action="store_true")
    parser.add_argument("-all", "--all", action="store_true")
    parser.add_argument("-f", "--force", action="store_true")
    parser.add_argument("-w", "--which", action="store_true",
                        help="see which jobs to go")
    parser.add_argument("-validate", action="store_true",
                        help="validate tasks")
    parser.add_argument(
        "-rt",
        "--remove_terminated",
        action="store_true",
        help="see which jobs are manually terminated and remove them",
    )
    parser.add_argument(
        "-rf",
        "--remove_failed",
        action="store_true",
        help="remove jobs with container failure",
    )
    args = parser.parse_args()

    submissions = args.submissions
    skipped_from_lock = 0
    skipped_from_log = 0
    skipped_from_mi = 0
    to_go = 0
    if args.remove_locks:
        os.system(f"rm -rf {CONFIGS['LOCK_DIR']}")
        print("Locks removed. Exiting.")
        return

    submitted = 0

    MODEL_CONFIGS = CONFIGS["MODEL_CONFIGS"]
    MODEL_CONFIG_VARS = list(MODEL_CONFIGS.keys())
    CONFIG_COMBINATIONS = get_model_config_combinations()
    DOMAINS = CONFIGS["DOMAINS"]
    PROBLEMS = CONFIGS["PROBLEMS"]

    for domain in Perc(DOMAINS):
        for config in CONFIG_COMBINATIONS:
            config_dict = dict(zip(MODEL_CONFIG_VARS, config))
            use_sas = config_dict["facts"] == "fd"
            model_description = "_".join(map(str, [domain] + list(config)))
            model_file = f"{CONFIGS['MODEL_DIR']}/{model_description}.model"
            if not os.path.exists(model_file):
                continue

            for problem in PROBLEMS:

                if use_sas:
                    domain_pddl = "fdr"
                    problem_pddl = f"{SAS_BENCHMARK_DIR}/{domain}_{problem}.sas"
                    planner_flag = "-p fd"
                    if not os.path.exists(problem_pddl):
                        continue
                else:
                    domain_pddl = f"{PDDL_BENCHMARK_DIR}/{domain}/domain.pddl"
                    problem_pddl = f"{PDDL_BENCHMARK_DIR}/{domain}/testing/{problem}.pddl"
                    planner_flag = "-p pwl"

                description = "_".join(
                    map(str, [domain, problem] + list(config)))

                tmp_dir = f"{CONFIGS['TMP_DIR']}/{description}"
                lock_file = f"{CONFIGS['LOCK_DIR']}/{description}.lck"
                log_file = f"{CONFIGS['LOG_DIR']}/{description}.log"

                if args.remove_terminated:
                    if not os.path.exists(log_file):
                        continue
                    with open(log_file) as f:
                        content = f.read()
                    if (
                        "SIGTERM Termination" in content
                        or "SIGKILL Kill" in content
                        or "DUE to SIGNAL Terminated" in content
                    ):
                        os.remove(log_file)
                        print(f"Removed {description}")
                    continue

                if args.remove_failed:
                    if not os.path.exists(log_file):
                        continue
                    with open(log_file) as f:
                        content = f.read()
                    if "FATAL:   container creation failed: mount" in content:
                        os.remove(log_file)
                        print(f"Removed {description}")
                    continue

                if os.path.exists(lock_file) and not args.force:
                    skipped_from_lock += 1
                    continue
                if os.path.exists(log_file) and not args.force:
                    skipped_from_log += 1
                    continue
                if args.which:
                    print(f"{description=}")
                    to_go += 1
                    continue
                if submitted >= submissions:
                    to_go += 1
                    continue

                cmd = f"apptainer run --bind {CUR_DIR}:{CUR_DIR} {GOOSE_SIF} plan {domain_pddl} {problem_pddl} {model_file} {planner_flag}"
                job_vars = ",".join([f"CMD={cmd}", f"TMP_DIR={tmp_dir}"])

                job_cmd = [
                    "sbatch",
                    f"--job-name=plan_{description}",
                    f"--output={log_file}",
                    f"--export={job_vars}",
                    JOB_SCRIPT,
                ]

                with open(lock_file, "w") as f:
                    f.write("")
                p = subprocess.Popen(job_cmd)
                p.wait()

                print(f" log: {log_file}")
                submitted += 1

    print("*" * 80)
    print(f"{submitted=}")
    print(f"{skipped_from_lock=}")
    print(f"{skipped_from_log=}")
    print(f"{skipped_from_mi=}")
    print(f"{to_go=}")
    print("*" * 80)


if __name__ == "__main__":
    main()
