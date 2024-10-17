import argparse
from benchmarking_utils import run_fd


def main():
    parser = argparse.ArgumentParser(
        description="Running fast-downward in different modes"
    )
    parser.add_argument("-d", "--domain", type=str, required=True)
    parser.add_argument("-i", "--instance", type=str, required=True)
    parser.add_argument(
        "-m", "--mode", choices=("blind", "lama", "opt"), default="lama"
    )
    parser.add_argument("-v", "--verbose", action="store_true")

    args = parser.parse_args()
    domain = args.domain
    instance = args.instance
    mode = args.mode
    verbose = args.verbose

    modes = {
        "blind": "--search astar(blind())",
        "lama": "--alias lama-first ",
        "opt": "--alias seq-opt-lmcut",
    }

    instance_id = instance.split("/")[-1].split(".")[0]
    if "blind" == mode:
        task = f"--plan-file {instance_id}.plan --sas-file {instance_id}.sas {domain} {instance} {modes[mode]}"
    else:  # alias
        task = f"{modes[mode]} --plan-file {instance_id}.plan --sas-file {instance_id}.sas {domain} {instance}"

    plan = run_fd(task=task, instance_id=instance_id, verbose=verbose)
    if plan and verbose:
        print(f"{plan}")


if __name__ == "__main__":
    main()
