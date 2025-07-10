#!/usr/bin/env python

import argparse
import os
import shutil
import subprocess


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cuda", action="store_true", help="Build CUDA version.")
    parser.add_argument("--clear-cache", action="store_true", help="Remove intermediate build 1.")
    args = parser.parse_args()

    CUR_DIR = os.path.dirname(os.path.abspath(__file__))
    ROOT_DIR = os.path.dirname(CUR_DIR)

    # Check if Apptainer is installed on system
    if not shutil.which("apptainer"):
        raise EnvironmentError(f"Apptainer is not installed. Please install it with `sudo apt install apptainer`.")

    suffix = "-cuda" if args.cuda else ""
    INTERMEDIATE_DEF_0 = f"{CUR_DIR}/docker-ubuntu22_04{suffix}.def"
    INTERMEDIATE_SIF_0 = f"{CUR_DIR}/docker-ubuntu22_04{suffix}.sif"
    INTERMEDIATE_DEF_1 = f"{CUR_DIR}/goose-base{suffix}.def"
    INTERMEDIATE_SIF_1 = f"{CUR_DIR}/goose-base{suffix}.sif"
    DEF = f"{CUR_DIR}/goose{suffix}.def"
    SIF = f"{ROOT_DIR}/goose{suffix}.sif"

    # Check if remove intermediate image
    if args.clear_cache and os.path.exists(INTERMEDIATE_SIF_1):
        print(f"Removing intermediate image: {INTERMEDIATE_SIF_1}")
        os.remove(INTERMEDIATE_SIF_1)

    # Build intermediate image 0
    if not os.path.exists(INTERMEDIATE_SIF_0):
        print(f"Building intermediate image: {INTERMEDIATE_SIF_0}")
        subprocess.call(["apptainer", "build", INTERMEDIATE_SIF_0, INTERMEDIATE_DEF_0], cwd=ROOT_DIR)
    else:
        print(f"Intermediate image {INTERMEDIATE_SIF_0} exists. Skipping intermediate build 0.")

    # Build intermediate image 1
    if not os.path.exists(INTERMEDIATE_SIF_1):
        print(f"Building intermediate image: {INTERMEDIATE_SIF_1}")
        subprocess.call(["apptainer", "build", INTERMEDIATE_SIF_1, INTERMEDIATE_DEF_1], cwd=ROOT_DIR)
    else:
        print(f"Intermediate image {INTERMEDIATE_SIF_1} exists. Skipping intermediate build 1.")

    # Build final image
    subprocess.call(["apptainer", "build", SIF, DEF], cwd=ROOT_DIR)


if __name__ == "__main__":
    main()
