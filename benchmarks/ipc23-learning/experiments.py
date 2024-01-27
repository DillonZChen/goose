import argparse
import logging
import os
import sys
import multiprocessing as mp
from tqdm import tqdm
from benchmarking_utils import execute_command


def parallel_execution(domain: str, instance_names: list[str], mode: str):
    print(
        f"Parallelizing {len(instance_names)} tasks with {mp.cpu_count()} processors"
    )
    pool = mp.Pool(mp.cpu_count())
    # pool = mp.Pool(4)
    pbar = tqdm(
        total=len(instance_names),
        bar_format="{percentage:3.0f}%|{bar:10}{r_bar}",
    )

    def collect_result(result):
        pbar.update()

    def print_error(result):
        print(f"\rError callback: {result}\n")

    for task in instance_names:
        command = f"python runner.py -d {domain} -i {task} -m {mode}".split()
        pool.apply_async(
            execute_command,
            kwds={"command": command},
            callback=collect_result,
            error_callback=print_error,
        )
    pool.close()
    pool.join()
    pbar.close()


def main():
    parser = argparse.ArgumentParser(
        description="Executing the runner.py over all instances in a dir"
    )
    parser.add_argument("-i", "--instances_dir", type=str, required=True)
    parser.add_argument(
        "-m", "--mode", choices=("blind", "lama", "opt"), default="lama"
    )
    args = parser.parse_args()
    instances_dir = args.instances_dir
    if instances_dir and instances_dir[-1] != "/":
        instances_dir += "/"
    domain = "/".join(instances_dir.split("/")[:-3]) + "/domain.pddl"
    mode = args.mode

    if not os.path.isdir(instances_dir):
        logging.error(f"{instances_dir} is not a valid directory")
        sys.exit(-1)

    # https://stackoverflow.com/questions/3207219/how-do-i-list-all-files-of-a-directory
    instance_names = next(os.walk(instances_dir), (None, None, []))[
        2
    ]  # [] if no file
    if instance_names:
        instance_names = [
            f"{instances_dir}{ins_name}"
            for ins_name in sorted(instance_names)
            if ins_name[-5:] == ".pddl"
        ]
    else:
        logging.error(f"No instances found at {instances_dir}")

    parallel_execution(domain=domain, instance_names=instance_names, mode=mode)


if __name__ == "__main__":
    main()
