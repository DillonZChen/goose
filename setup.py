import argparse
import os

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("device", choices=["gpu", "cpu"], help="device for training and testing")
    args = parser.parse_args()

    container_name = f"goose_{args.device}"
    container = f"{container_name}.sif"
    if not os.path.exists(container):
        print(f"error: container {container} not built!")
        print(f"build it with `sudo singularity build {container_name}.sif {container_name}.def'")
        exit(-1)

    for planner in ["downward", "powerlifted"]:
        os.system(f"cd planners/{planner} && ../../{container} python3 build.py")
