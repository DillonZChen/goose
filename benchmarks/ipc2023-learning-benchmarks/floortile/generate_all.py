from benchmarking_utils import execute_command
import logging
from queue import PriorityQueue

def get_next_config(starting_grid: int = 2,
                    step_grid: int = 1,
                    starting_robots: int = 1,
                    max_robots: int = 1,
                    out_folder: str = ".",
                    starting_instance_id: int = 1,
                    max_instance_id: int = 100,
                    seed: int = 42):
    instance_id, steps = starting_instance_id, 0
    num_instances = float(1+max_instance_id-instance_id)
    step_robots = float(1+max_robots-starting_robots) / num_instances
    current_grid = (starting_grid*starting_grid, starting_grid, starting_grid)
    grids = PriorityQueue()  # prioritize grids by its area (smaller preferred)
    grids.put(current_grid)
    # visited_grids = set()
    # visited_grids.add(current_grid)
    while instance_id <= max_instance_id:
        _, rows, columns = grids.get()
        robots = min(int(starting_robots + step_robots * steps), columns)  # constraint: robots <= columns
        print(f"r={rows}; c={columns}; p={robots}")
        yield f"PYTHONHASHSEED=0 python floortile.py -r {rows} -c {columns} -p {robots} -o {out_folder} -id {instance_id} --seed {seed}"
        # Update input values for the next instance
        instance_id += 1
        seed += 1
        steps += 1
        next_grids = [((rows+step_grid)*columns, rows+step_grid, columns),
                      (rows*(columns+step_grid), rows, columns+step_grid),
                      ((rows+step_grid)*(columns+step_grid), rows+step_grid, columns+step_grid)]
        for ng in next_grids:
            # if not (ng in visited_grids):
            #    visited_grids.add(ng)
            grids.put(ng)  # allow repeated grids


def main():
    # Easy instances are still quite hard to solve, even for LAMA
    starting_grid = [3, 3, 10, 25]
    step_grid = [1, 1, 3, 3]
    starting_robots = [1, 1, 4, 15]
    max_robots = [3, 3, 15, 35]
    output_folders = ["training/easy", "testing/easy", "testing/medium", "testing/hard"]
    max_ids = [99, 30, 30, 30]
    init_ids = [17, 1, 1, 1]  # 16 base cases
    seeds = [42, 1007, 1007, 1007]
    for experiment in range(4):
        # print(output_folders[experiment])
        for command in get_next_config(
                starting_grid=starting_grid[experiment],
                step_grid=step_grid[experiment],
                starting_robots=starting_robots[experiment],
                max_robots=max_robots[experiment],
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
