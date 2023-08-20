import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import re 
from representation import REPRESENTATIONS

""" Module containing useful methods and configurations for 24-AAAI search experiments. """


REPEATS = 1
VAL_REPEATS = 5
TIMEOUT = 620  # 10 minute timeout + time to load model etc.
FAIL_LIMIT = {
  "gripper": 1,
  "spanner": 10,
  "visitall": 10,
  "visitsome": 10,
  "blocks": 10,
  "ferry": 10,
  "sokoban": 20,
  "n-puzzle": 10,
}


def sorted_nicely( l ): 
  """ Sort the given iterable in the way that humans expect.""" 
  convert = lambda text: int(text) if text.isdigit() else text 
  alphanum_key = lambda key: [ convert(c) for c in re.split('([0-9]+)', key) ] 
  return sorted(l, key = alphanum_key)

def search_cmd(df, pf, m, model_type, planner, search, seed, timeout=TIMEOUT):
  search_engine = {
    "pwl": pwl_cmd,
    "fd": fd_cmd,
  }[planner]
  return search_engine(df, pf, model_type, m, search, seed, timeout)

def pwl_cmd(df, pf, model_type, m, search, seed, timeout=TIMEOUT):
  os.makedirs("lifted", exist_ok=True)
  os.makedirs("plans", exist_ok=True)
  description = f"pwl_{pf.replace('.pddl','').replace('/','-')}_{search}_{os.path.basename(m).replace('.dt', '')}"
  lifted_file = f"lifted/{description}.lifted"
  plan_file = f"plans/{description}.plan"
  cmd = f"./../powerlifted/powerlifted.py --gpu " \
        f"-d {df} " \
        f"-i {pf} " \
        f"-m {m} " \
        f"-e gnn " \
        f"-s {search} " \
        f"--time-limit {timeout} " \
        f"--seed {seed} " \
        f"--translator-output-file {lifted_file} " \
        f"--plan-file {plan_file}"
  cmd = f"export GOOSE={os.getcwd()} && {cmd}"
  return cmd, lifted_file

def fd_cmd(df, pf, model_type, m, search, seed, timeout=TIMEOUT):
  os.makedirs("sas_files", exist_ok=True)
  os.makedirs("plans", exist_ok=True)
  
  if search == "gbbfs":
    search = "batch_eager_greedy"
  else:
    raise NotImplementedError
  
  description = f"fd_{pf.replace('.pddl','').replace('/','-')}_{search}_{os.path.basename(m).replace('.dt', '')}"
  sas_file = f"sas_files/{description}.sas_file"
  plan_file = f"plans/{description}.plan"
  cmd = f"./../downward/fast-downward.py --search-time-limit {timeout} --sas-file {sas_file} --plan-file {plan_file} "+\
        f"{df} {pf} --search '{search}([goose(model_path=\"{m}\", "+\
                                            f"model_type=\"{model_type}\", "+\
                                            f"domain_file=\"{df}\", "+\
                                            f"instance_file=\"{pf}\""+\
                                            f")])'"
  cmd = f"export GOOSE={os.getcwd()} && {cmd}"
  return cmd, sas_file
