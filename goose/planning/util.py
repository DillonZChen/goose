import logging
import os
import re
import subprocess
import tempfile

import pddl

from goose.util.paths import PLANNERS_DIR


def is_domain_numeric(domain_pddl: str) -> bool:
    pddl_domain = pddl.parse_domain(domain_pddl)
    return len(pddl_domain.functions) > 0


def get_downward_translation_atoms(domain_pddl: str, problem_pddl: str) -> set[str]:
    """Calls Downward translator to get ground atoms.

    Parameters
    ----------

    domain_pddl : str
        The domain PDDL file.

    problem_pddl : str
        The problem PDDL file.

    """

    script = f"{PLANNERS_DIR}/scorpion/fast-downward.py"

    with tempfile.TemporaryDirectory() as temp_dir:
        sas_file = f"{temp_dir}/output.sas"
        cmd = [script, "--sas-file", sas_file, "--translate", domain_pddl, problem_pddl]
        subprocess.check_output(cmd, universal_newlines=True)
        with open(sas_file, "r") as f:
            content = f.read()

    atoms = set(re.findall(r"Atom (.+)\n", content))

    return atoms


def call_numeric_downward(domain_pddl: str, problem_pddl: str, config: str) -> str:
    """Calls Numeric Downward and returns the command line output.

    Parameters
    ----------

    domain_pddl : str
        The domain PDDL file.

    problem_pddl : str
        The problem PDDL file.

    config : str
        The configuration for the search algorithm.

    """

    # check if built yet
    nfd_path = f"{PLANNERS_DIR}/numeric-downward"
    build_path = f"{nfd_path}/builds"
    if not os.path.exists(build_path):
        print("Error: Numeric Fast Downward not built yet. You can build it with `./build nfd`.")
        print("Terminating...")
        exit(-1)

    process = subprocess.Popen(["lscpu"], stdout=subprocess.PIPE)
    output, error = process.communicate()
    output = output.decode()

    with tempfile.TemporaryDirectory() as temp_dir:
        sas_file = f"{temp_dir}/output.sas"

        # call NFD
        cmd = [
            "python2",
            f"{nfd_path}/fast-downward.py",
            "--build",
            "release64",
            "--sas_file",
            sas_file,
            domain_pddl,
            problem_pddl,
            "--search",
            config,
        ]
        pipe = subprocess.PIPE
        process = subprocess.Popen(cmd, stdout=pipe, stderr=pipe)
        output, error = process.communicate()
        output = output.decode()
        error = error.decode()
        if process.returncode != 0:
            logging.info("Encountered error in call_numeric_downward\n" + error)
            exit(process.returncode)

    # print(output, flush=True)
    # print(error, flush=True)

    return output
