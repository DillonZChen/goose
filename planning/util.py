import datetime
import logging
import os
import re
import subprocess
from typing import Optional

import pddl

_CUR_DIR = os.path.dirname(os.path.abspath(__file__))
PLANNERS_DIR = f"{_CUR_DIR}/ext"


def is_domain_numeric(domain_pddl: str) -> bool:
    pddl_domain = pddl.parse_domain(domain_pddl)
    return len(pddl_domain.functions) > 0


def get_downward_translation_atoms(domain_pddl: str, problem_pddl: str, hash_prefix: str):
    script = f"{PLANNERS_DIR}/downward/fast-downward.py"
    sas_file = str(hash(domain_pddl)) + "_" + str(hash(problem_pddl))
    sas_file = sas_file.replace("-", "0") + ".sas"
    sas_file = hash_prefix.replace("-", "0") + "_" + sas_file
    cmd = [script, "--sas-file", sas_file, "--translate", domain_pddl, problem_pddl]
    subprocess.check_output(cmd, universal_newlines=True)
    with open(sas_file, "r") as f:
        content = f.read()
    os.remove(sas_file)
    atoms = re.findall(r"Atom (.+)\n", content)
    atoms = set(atoms)
    return atoms


def call_numeric_downward(
    domain_pddl: str,
    problem_pddl: str,
    config: str,
    hash_prefix: Optional[str] = None,
):
    """Calls Numeric Downward and returns the command line output.

    Parameters
    ----------

    domain_pddl : str
        The domain PDDL file.

    problem_pddl : str
        The problem PDDL file.

    config : str
        The configuration for the search algorithm.

    hash_prefix : Optional[str], optional
        The prefix to use for the hash of the tmp file.

    """

    # check if built yet
    nfd_path = f"{PLANNERS_DIR}/numeric-downward"
    build_path = f"{nfd_path}/builds"
    if not os.path.exists(build_path):
        print("Error: Numeric Fast Downward not built yet. You can build it with `sh build.sh`.")
        print("Terminating...")
        exit(-1)

    # try to generate a unique tmp file name in case trainers are parallelised
    if hash_prefix is None:
        hash_prefix = datetime.datetime.now().strftime("%I:%M%p on %B %d, %Y")
    process = subprocess.Popen(["lscpu"], stdout=subprocess.PIPE)
    output, error = process.communicate()
    output = output.decode()
    sas_file = domain_pddl + problem_pddl + hash_prefix + repr(hash(output))
    for tok in ["/", ".", "-", " ", "\n"]:
        sas_file = sas_file.replace(tok, "_")
    os.makedirs(".tmp", exist_ok=True)
    sas_file = f".tmp/{sas_file}.sas"

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
