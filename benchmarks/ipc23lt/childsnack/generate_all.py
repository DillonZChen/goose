from benchmarking_utils import execute_command
import logging


def get_next_config(
    starting_children: int = 1,
    max_children: int = 1,
    starting_allergic: int = 0,
    max_allergic: int = 1,
    starting_trays: int = 1,
    max_trays: int = 1,
    starting_sandwiches: int = 1,
    max_sandwiches: int = 1,
    out_folder: str = ".",
    starting_instance_id: int = 1,
    max_instance_id: int = 100,
    seed: int = 42,
):
    instance_id, steps = starting_instance_id, 0
    num_instances = float(1 + max_instance_id - starting_instance_id)
    step_children = float(1 + max_children - starting_children) / num_instances
    step_allergic = float(1 + max_allergic - starting_allergic) / num_instances
    step_trays = float(1 + max_trays - starting_trays) / num_instances
    step_sandwiches = (
        float(1 + max_sandwiches - starting_sandwiches) / num_instances
    )
    while instance_id <= max_instance_id:
        children = int(step_children * steps + starting_children)
        allergic = int(step_allergic * steps + starting_allergic)
        trays = int(step_trays * steps + starting_trays)
        sandwiches = int(step_sandwiches * steps + starting_sandwiches)
        assert allergic <= children <= sandwiches
        print(f"c={children}; a={allergic}; t={trays}; s={sandwiches}")
        yield f"PYTHONHASHSEED=0 python childsnack.py -c {children} -a {allergic} -t {trays} -s {sandwiches} -o {out_folder} -i {instance_id} --seed {seed}"
        # Update input values for the next instance
        instance_id += 1
        steps += 1
        seed += 1


def main():
    starting_children = [4, 4, 15, 50]
    max_children = [10, 10, 40, 300]
    starting_allergic = [0, 0, 15, 50]
    max_allergic = [6, 6, 25, 150]  # approx max_children*0.5
    starting_trays = [1, 1, 2, 4]
    max_trays = [3, 3, 5, 10]
    starting_sandwiches = [4, 4, 15, 50]
    max_sandwiches = [15, 15, 60, 450]  # max_children * 1.5
    output_folders = [
        "training/easy",
        "testing/easy",
        "testing/medium",
        "testing/hard",
    ]
    max_ids = [99, 30, 30, 30]
    init_ids = [14, 1, 1, 1]  # 13 base cases
    seeds = [42, 1007, 1007, 1007]
    for experiment in range(4):
        # print(output_folders[experiment])
        for command in get_next_config(
            starting_children=starting_children[experiment],
            max_children=max_children[experiment],
            starting_allergic=starting_allergic[experiment],
            max_allergic=max_allergic[experiment],
            starting_trays=starting_trays[experiment],
            max_trays=max_trays[experiment],
            starting_sandwiches=starting_sandwiches[experiment],
            max_sandwiches=max_sandwiches[experiment],
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
