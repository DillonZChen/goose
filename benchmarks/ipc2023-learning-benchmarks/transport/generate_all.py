from benchmarking_utils import execute_command
import logging


def get_next_config(starting_vehicles: int = 1,
                    max_vehicles: int = 1,
                    starting_packages: int = 1,
                    max_packages: int = 1,
                    starting_locations: int = 2,
                    max_locations: int = 1,
                    starting_max_capacity: int = 1,
                    max_max_capacity: int = 1,
                    out_folder: str = ".",
                    starting_instance_id = 1,
                    max_instance_id: int = 100,
                    seed: int = 42):
    instance_id, steps = 1, 0
    num_instances = float(1+max_instance_id-starting_instance_id)
    step_vehicles = float(1+max_vehicles - starting_vehicles) / num_instances
    step_packages = float(1+max_packages - starting_packages) / num_instances
    step_locations = float(1+max_locations - starting_locations) / num_instances
    step_max_capacity = float(1+max_max_capacity - starting_max_capacity) / num_instances
    while instance_id <= max_instance_id:
        vehicles = int(step_vehicles * steps + starting_vehicles)
        packages = int(step_packages * steps + starting_packages)
        locations = int(step_locations * steps + starting_locations)
        max_capacity = int(step_max_capacity * steps + starting_max_capacity)
        print(f"v={vehicles}; p={packages}; l={locations}; m={max_capacity}")
        yield f"PYTHONHASHSEED=0 python transport.py -v {vehicles} -p {packages} -l {locations} -m {max_capacity} -o {out_folder} -i {instance_id} --seed {seed}"
        # Update input values for the next instance
        instance_id += 1
        seed += 1
        steps += 1


def main():
    starting_vehicles = [3, 3, 10, 30]
    max_vehicles = [6, 6, 20, 50]
    starting_packages = [1, 1, 5, 20]
    max_packages = [15, 15, 45, 200]
    starting_locations = [5, 5, 20, 50]
    max_locations = [15, 15, 40, 100]
    starting_max_capacity = [2, 2, 4, 10]
    max_max_capacity = [2, 2, 4, 10]
    output_folders = ["training/easy", "testing/easy", "testing/medium", "testing/hard"]
    max_ids = [99, 30, 30, 30]
    init_ids = [11, 1, 1, 1]  # 10 base cases
    seeds = [42, 1007, 1007, 1007]
    for experiment in range(4):
        # print(output_folders[experiment])
        for command in get_next_config(
                starting_vehicles=starting_vehicles[experiment],
                max_vehicles=max_vehicles[experiment],
                starting_packages=starting_packages[experiment],
                max_packages=max_packages[experiment],
                starting_locations=starting_locations[experiment],
                max_locations=max_locations[experiment],
                starting_max_capacity=starting_max_capacity[experiment],
                max_max_capacity=max_max_capacity[experiment],
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
