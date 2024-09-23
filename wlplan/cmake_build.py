#!/usr/bin/env python

import argparse
import subprocess

exec(open("wlplan/__version__.py").read())


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("installation_path", type=str)
    args = parser.parse_args()

    subprocess.check_call(["cmake", "-S", ".", "-B", "build", f"-DWLPLAN_VERSION={__version__}"])
    subprocess.check_call(["cmake", "--build", "build", "-j32"])
    subprocess.check_call(["cmake", "--install", "build", "--prefix", args.installation_path])


if __name__ == "__main__":
    main()
