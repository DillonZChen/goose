from benchmarking_utils import execute_command
import logging


def get_next_config(
    starting_spanners: int = 1,
    max_spanners: int = 1,
    starting_nuts: int = 1,
    max_nuts: int = 1,
    starting_locations: int = 1,
    max_locations: int = 1,
    out_folder: str = ".",
    starting_instance_id: int = 1,
    max_instance_id: int = 100,
    seed: int = 42,
):
    instance_id, steps = starting_instance_id, 0
    num_instances = float(1 + max_instance_id - starting_instance_id)
    step_spanners = float(1 + max_spanners - starting_spanners) / num_instances
    step_nuts = float(1 + max_nuts - starting_nuts) / num_instances
    step_locations = (
        float(1 + max_locations - starting_locations) / num_instances
    )
    while instance_id <= max_instance_id:
        spanners = int(step_spanners * steps + starting_spanners)
        nuts = int(step_nuts * steps + starting_nuts)
        locations = int(step_locations * steps + starting_locations)
        print(f"s={spanners}; n={nuts}; l={locations}")
        yield f"PYTHONHASHSEED=0 python spanner.py -s {spanners} -n {nuts} -l {locations} -o {out_folder} -i {instance_id} --seed {seed} "
        # Update input values for the next instance
        instance_id += 1
        seed += 1
        steps += 1


def main():
    starting_spanners = [1, 1, 30, 100]
    max_spanners = [10, 10, 90, 500]
    starting_nuts = [1, 1, 15, 50]
    max_nuts = [5, 5, 50, 250]
    starting_locations = [4, 4, 15, 50]
    max_locations = [10, 10, 45, 100]
    output_folders = [
        "training/easy",
        "testing/easy",
        "testing/medium",
        "testing/hard",
    ]
    max_ids = [99, 30, 30, 30]
    init_ids = [15, 1, 1, 1]  # 14 base cases
    seeds = [42, 1007, 1007, 1007]
    for experiment in range(4):
        for command in get_next_config(
            starting_spanners=starting_spanners[experiment],
            max_spanners=max_spanners[experiment],
            starting_nuts=starting_nuts[experiment],
            max_nuts=max_nuts[experiment],
            starting_locations=starting_locations[experiment],
            max_locations=max_locations[experiment],
            out_folder=output_folders[experiment],
            starting_instance_id=init_ids[experiment],
            max_instance_id=max_ids[experiment],
            seed=seeds[experiment],
        ):
            ret_code = execute_command(command=command, shell=True)
            logging.info(f"Executed command={command}; result={ret_code}")

    # Copy base cases
    command = "cp base_cases/* training/easy/"
    ret_code = execute_command(command=command, shell=True)
    logging.info(f"Executed command={command}; result={ret_code}")


if __name__ == "__main__":
    main()
