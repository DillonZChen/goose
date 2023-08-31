from benchmarking_utils import execute_command
import logging


def get_next_config(starting_satellites: int = 1,
                    max_satellites: int = 1,
                    starting_instruments: int = 1,
                    max_instruments: int = 1,
                    starting_modes: int = 1,
                    max_modes: int = 1,
                    starting_directions: int = 2,
                    max_directions: int = 1,
                    out_folder: str = ".",
                    starting_instance_id: int = 1,
                    max_instance_id: int = 100,
                    seed: int = 42):
    instance_id, steps = starting_instance_id, 0
    num_instances = float(1+max_instance_id-starting_instance_id)
    step_satellites = float(1+max_satellites-starting_satellites) / num_instances
    step_instruments = float(1+max_instruments - starting_instruments) / num_instances
    step_modes = float(1+max_modes - starting_modes) / num_instances
    step_directions = float(1+max_directions - starting_directions) / num_instances
    while instance_id <= max_instance_id:
        satellites = int(step_satellites * steps + starting_satellites)
        instruments = int(step_instruments * steps + starting_instruments)
        modes = int(step_modes * steps + starting_modes)
        directions = int(step_directions * steps + starting_directions)
        print(f"s={satellites}; i={instruments}; m={modes}; d={directions}")
        yield f"PYTHONHASHSEED=0 python satellite.py -s {satellites} -i {instruments} -m {modes} -d {directions} " \
              f"-o {out_folder} -id {instance_id} --seed {seed}"
        # Update input values for the next instance
        instance_id += 1
        seed += 1
        steps += 1


def main():
    starting_satellites = [3, 3, 15, 50]
    max_satellites = [10, 10, 40, 100]
    starting_instruments = [3, 3, 15, 50]
    max_instruments = [20, 20, 80, 200]  # max_satellites * 2
    starting_modes = [1, 1, 3, 5]
    max_modes = [3, 3, 5, 10]
    starting_directions = [4, 4, 15, 40]
    max_directions = [10, 10, 30, 100]
    output_folders = ["training/easy", "testing/easy", "testing/medium", "testing/hard"]
    max_ids = [99, 30, 30, 30]
    init_ids = [9, 1, 1, 1]  # 8 base cases
    seeds = [42, 1007, 1007, 1007]
    for experiment in range(4):
        # print(output_folders[experiment])
        for command in get_next_config(
                starting_satellites=starting_satellites[experiment],
                max_satellites=max_satellites[experiment],
                starting_instruments=starting_instruments[experiment],
                max_instruments=max_instruments[experiment],
                starting_modes=starting_modes[experiment],
                max_modes=max_modes[experiment],
                starting_directions=starting_directions[experiment],
                max_directions=max_directions[experiment],
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
