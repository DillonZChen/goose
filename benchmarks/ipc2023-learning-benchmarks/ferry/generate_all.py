from benchmarking_utils import execute_command
import logging


def get_next_config(starting_cars: int = 1,
                    max_cars: int = 1,
                    starting_locations: int = 1,
                    max_locations: int = 1,
                    out_folder: str = ".",
                    starting_instance_id: int = 1,
                    max_instance_id: int = 100,
                    seed: int = 42):
    instance_id, steps = starting_instance_id, 0
    num_instances = float(1+max_instance_id-instance_id)
    step_cars = float(1+max_cars-starting_cars) / num_instances
    step_locations = float(1+max_locations-starting_locations) / num_instances
    while instance_id <= max_instance_id:
        cars = int(starting_cars + step_cars * steps)
        locations = int(starting_locations + step_locations * steps)
        print(f"c={cars}; l={locations}")
        yield f"PYTHONHASHSEED=0 python ferry.py -c {cars} -l {locations} -o {out_folder} -i {instance_id} --seed {seed}"
        # Update input values for the next instance
        steps += 1
        instance_id += 1
        seed += 1
    # raise StopIteration()


def main():
    starting_cars = [1, 2, 10, 200]
    max_cars = [20, 20, 100, 1000]
    starting_locations = [5, 5, 20, 100]
    max_locations = [15, 15, 50, 500]
    output_folders = ["training/easy", "testing/easy", "testing/medium", "testing/hard"]
    max_ids = [99, 30, 30, 30]
    init_ids = [12, 1, 1, 1]  # 11 base cases
    seeds = [42, 1007, 1007, 1007]
    for experiment in range(4):
        # print(output_folders[experiment])
        for command in get_next_config(
                starting_cars=starting_cars[experiment],
                max_cars=max_cars[experiment],
                starting_locations=starting_locations[experiment],
                max_locations=max_locations[experiment],
                out_folder=output_folders[experiment],
                starting_instance_id=init_ids[experiment],
                max_instance_id=max_ids[experiment],
                seed=seeds[experiment]):
            ret_code = execute_command(command=command, shell=True)
            logging.info(f"Executed command={command}; result={ret_code}")

    # Copy base cases
    command = "cp base_cases/* training/easy/"
    ret_code = execute_command(command=command, shell=True)
    logging.info(f"Executed command={command}; result={ret_code}")


if __name__ == "__main__":
    main()
