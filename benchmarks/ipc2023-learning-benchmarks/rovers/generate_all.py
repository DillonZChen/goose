from benchmarking_utils import execute_command
import logging


def get_next_config(starting_rovers: int = 1,
                    max_rovers: int = 1,
                    starting_waypoints: int = 2,
                    max_waypoints: int = 1,
                    starting_cameras: int = 1,
                    max_cameras: int = 1,
                    starting_objectives: int = 1,
                    max_objectives: int = 1,
                    out_folder: str = ".",
                    starting_instance_id: int = 1,
                    max_instance_id: int = 100,
                    seed: int = 42):
    instance_id, steps = starting_instance_id, 0
    num_instances = float(1+max_instance_id-starting_instance_id)
    step_rovers = float(1+max_rovers-starting_rovers) / num_instances
    step_waypoints = float(1+max_waypoints-starting_waypoints) / num_instances
    step_cameras = float(1+max_cameras - starting_cameras) / num_instances
    step_objectives = float(1 + max_objectives - starting_cameras) / num_instances

    while instance_id <= max_instance_id:
        rovers = int(step_rovers * steps + starting_rovers)
        waypoints = int(step_waypoints * steps + starting_waypoints)
        cameras = int(step_cameras * steps + starting_cameras)
        objectives = int(step_objectives * steps + starting_objectives)
        print(f"r={rovers}; w={waypoints}; c={cameras}; o={objectives}")
        yield f"PYTHONHASHSEED=0 python rovers.py -r {rovers} -w {waypoints} -c {cameras} -o {objectives} " \
              f"-out {out_folder} -id {instance_id} --seed {seed}"
        # Update input values for the next instance
        instance_id += 1
        seed += 1
        steps += 1
        """
        if (waypoints + step_waypoints) <= (rovers * 5):
            waypoints += step_waypoints
        elif (cameras + step_cameras) <= (rovers*3):
            waypoints = starting_waypoints
            cameras += step_cameras
        elif (objectives + step_objectives) <= (cameras*2):
            waypoints = starting_waypoints
            cameras = starting_cameras
            objectives += step_objectives
        else:
            waypoints = starting_waypoints
            cameras = starting_cameras
            objectives = starting_objectives
            rovers += step_rovers
        """
    # raise StopIteration()


def main():
    starting_rovers = [1, 1, 5, 15]
    max_rovers = [4, 4, 10, 30]
    starting_waypoints = [4, 4, 15, 100]
    max_waypoints = [10, 10, 90, 200]
    starting_cameras = [1, 1, 5, 60]
    max_cameras = [4, 4, 50, 100]
    starting_objectives = [1, 1, 15, 100]
    max_objectives = [10, 10, 80, 200]
    init_ids = [7, 1, 1, 1]  # 6 base cases
    output_folders = ["training/easy", "testing/easy", "testing/medium", "testing/hard"]
    max_ids = [99, 30, 30, 30]
    seeds = [42, 1007, 1007, 1007]
    for experiment in range(4):
        # print(output_folders[experiment])
        for command in get_next_config(
                starting_rovers=starting_rovers[experiment],
                max_rovers=max_rovers[experiment],
                starting_waypoints=starting_waypoints[experiment],
                max_waypoints=max_waypoints[experiment],
                starting_cameras=starting_cameras[experiment],
                max_cameras=max_cameras[experiment],
                starting_objectives=starting_objectives[experiment],
                max_objectives=max_objectives[experiment],
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
