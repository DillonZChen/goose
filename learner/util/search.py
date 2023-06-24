import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import re 

from representation.config import CONFIG


REPEATS = 1
VAL_REPEATS = 5
TIMEOUT = 600
FAIL_LIMIT = {
  "gripper": 1,
  "spanner": 5,
  "visitall": 5,
  "visitsome": 5,
  "blocks": 5,
  "ferry": 5,
  "sokoban": 5,
  "n-puzzle": 5,
}


def sorted_nicely( l ): 
    """ Sort the given iterable in the way that humans expect.""" 
    convert = lambda text: int(text) if text.isdigit() else text 
    alphanum_key = lambda key: [ convert(c) for c in re.split('([0-9]+)', key) ] 
    return sorted(l, key = alphanum_key)

def search_cmd(rep, domain_name, df, pf, m, search, seed, timeout=TIMEOUT):
  if CONFIG[rep]["lifted"]:
    search_engine = pwl_cmd
  else:
    search_engine = fd_cmd
  return search_engine(rep, domain_name, df, pf, m, search, seed, timeout)

def pwl_cmd(rep, domain_name, df, pf, m, search, seed, timeout=TIMEOUT):
  os.makedirs("lifted", exist_ok=True)
  os.makedirs("plans", exist_ok=True)
  description = f"pwl_{domain_name}_{os.path.basename(pf).replace('.pddl','')}_{search}_{os.path.basename(m).replace('.dt', '')}"
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

def fd_cmd(rep, domain_name, df, pf, m, search, seed, timeout=TIMEOUT):
  os.makedirs("lifted", exist_ok=True)
  os.makedirs("plans", exist_ok=True)
  
  if search == "gbbfs":
    search = "batch_eager_greedy"
  else:
    raise NotImplementedError
  
  # 0: slg, 1: flg, 2: llg, 3: glg
  if rep == "sdg-el":   
    config = 0
    config_file = "slg"
  elif rep == "fdg-el": 
    config = 1
    config_file = "flg"
  else: 
    raise NotImplementedError
  
  description = f"fd_{domain_name}_{os.path.basename(pf).replace('.pddl','')}_{search}_{os.path.basename(m).replace('.dt', '')}"
  sas_file = f"lifted/{description}.sas_file"
  plan_file = f"plans/{description}.plan"
  with open(config_file, 'w') as f:
    f.write(m+'\n')
    f.write(df+'\n')
    f.write(pf+'\n')
    f.close()
  cmd = f'./../downward/fast-downward.py --sas-file {sas_file} --plan-file {plan_file} {df} {pf} --search "{search}([goose(graph={config})])"'
  cmd = f"export GOOSE={os.getcwd()} && {cmd}"
  return cmd, sas_file
