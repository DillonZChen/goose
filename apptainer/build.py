#!/usr/bin/env python

import argparse
import os
import shutil
import subprocess


def main():
    parser = argparse.ArgumentParser()
    mutex = parser.add_mutually_exclusive_group(required=False)
    mutex.add_argument("--scorp", action="store_true", help="Scorpion version.")
    mutex.add_argument("--cuda", action="store_true", help="Build CUDA version.")
    parser.add_argument("--clear-cache", action="store_true", help="Remove intermediate build 1.")
    args = parser.parse_args()

    CUR_DIR = os.path.dirname(os.path.abspath(__file__))
    ROOT_DIR = os.path.dirname(CUR_DIR)

    # Check if Apptainer is installed on system
    if not shutil.which("apptainer"):
        raise EnvironmentError(f"Apptainer is not installed. Please install it with `sudo apt install apptainer`.")

    suffix = "-cuda" if args.cuda else ""
    INTERMEDIATE_DEF = f"{CUR_DIR}/docker-ubuntu22_04{suffix}.def"
    INTERMEDIATE_SIF = f"{CUR_DIR}/docker-ubuntu22_04{suffix}.sif"
    suffix = "-scorp" if args.scorp else suffix
    DEF = f"{CUR_DIR}/goose{suffix}.def"
    SIF = f"{ROOT_DIR}/goose{suffix}.sif"

    # Check if remove intermediate image
    if args.clear_cache and os.path.exists(INTERMEDIATE_SIF):
        print(f"Removing intermediate image: {INTERMEDIATE_SIF}", flush=True)
        os.remove(INTERMEDIATE_SIF)

    # Build intermediate image 0
    if not os.path.exists(INTERMEDIATE_SIF):
        print(f"Building intermediate image: {INTERMEDIATE_SIF}", flush=True)
        subprocess.call(["apptainer", "build", INTERMEDIATE_SIF, INTERMEDIATE_DEF], cwd=ROOT_DIR)
    else:
        print(f"Intermediate image {INTERMEDIATE_SIF} exists. Skipping intermediate build.", flush=True)

    # Build final image
    subprocess.call(["apptainer", "build", SIF, DEF], cwd=ROOT_DIR)

    # Assert build succeeded
    if not os.path.exists(SIF):
        raise RuntimeError(f"Failed to build image: {SIF}")


if __name__ == "__main__":
    main()
