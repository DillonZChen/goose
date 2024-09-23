from benchmarking_utils import execute_command
import logging


def get_next_config(
    starting_grid_size: int = 4,
    max_grid_size: int = 1,
    starting_boxes: int = 1,
    max_boxes: int = 1,
    out_folder: str = ".",
    starting_instance_id: int = 1,
    max_instance_id: int = 100,
    seed: int = 42,
):
    instance_id, steps = starting_instance_id, 0
    num_instances = float(1 + max_instance_id - starting_instance_id)
    step_grid_size = (
        float(1 + max_grid_size - starting_grid_size) / num_instances
    )
    step_boxes = float(1 + max_boxes - starting_boxes) / num_instances
    while instance_id <= max_instance_id:
        grid_size = int(step_grid_size * steps + starting_grid_size)
        boxes = int(step_boxes * steps + starting_boxes)
        print(f"g={grid_size}; b={boxes}; seed={seed}")
        yield f"PYTHONHASHSEED=0 python sokoban.py -g {grid_size} -b {boxes} -out {out_folder} -id {instance_id} --seed {seed}"
        # Update input values for the next instance
        instance_id += 1
        seed += 1
        steps += 1


def main():
    starting_grid_size = [8, 8, 20, 60]
    max_grid_size = [13, 13, 50, 100]
    starting_boxes = [1, 1, 5, 40]
    max_boxes = [4, 4, 35, 80]
    output_folders = [
        "training/easy",
        "testing/easy",
        "testing/medium",
        "testing/hard",
    ]
    max_ids = [99, 30, 30, 30]
    init_ids = [13, 1, 1, 1]  # 12 base cases
    seeds = [42, 1007, 1007, 1007]
    for experiment in range(4):
        # print(output_folders[experiment])
        for command in get_next_config(
            starting_grid_size=starting_grid_size[experiment],
            max_grid_size=max_grid_size[experiment],
            starting_boxes=starting_boxes[experiment],
            max_boxes=max_boxes[experiment],
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
