import os
import re
import subprocess

_CUR_DIR = os.path.dirname(os.path.abspath(__file__))


def get_downward_translation_atoms(domain_pddl: str, problem_pddl: str, hash_prefix: str):
    script = f"{_CUR_DIR}/downward/fast-downward.py"
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
