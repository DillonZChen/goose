import os
import re 
from util.save_load import load_kernel_model_and_setup

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

PROFILE_CMD_ = "valgrind --tool=callgrind --callgrind-out-file=callgrind.out --dump-instr=yes --collect-jumps=yes"


def sorted_nicely( l ): 
  """ Sort the given iterable in the way that humans expect.""" 
  convert = lambda text: int(text) if text.isdigit() else text 
  alphanum_key = lambda key: [ convert(c) for c in re.split('([0-9]+)', key) ] 
  return sorted(l, key = alphanum_key)

def search_cmd(df, pf, m, model_type, planner, search, seed, profile, timeout=TIMEOUT, aux_file=None, plan_file=None):
  search_engine = {
    "pwl": pwl_cmd,
    "fd": fd_cmd,
  }[planner]
  cmd, aux_file = search_engine(df, pf, model_type, m, search, seed, profile, timeout, aux_file, plan_file)
  cmd = f"export GOOSE={os.getcwd()} && {cmd}"
  return cmd, aux_file

def pwl_cmd(df, pf, model_type, m, search, seed, profile, timeout=TIMEOUT, aux_file=None, plan_file=None):
  description = f"pwl_{pf.replace('.pddl','').replace('/','-')}_{search}_{os.path.basename(m).replace('.dt', '')}".replace('.', '')

  if aux_file is None:
    os.makedirs("lifted", exist_ok=True)
    aux_file = f"lifted/{description}.lifted"

  if plan_file is None:
    os.makedirs("plans", exist_ok=True)
    plan_file = f"plans/{description}.plan"

  cmd = f"./../powerlifted/powerlifted.py --gpu " \
        f"-d {df} " \
        f"-i {pf} " \
        f"-m {m} " \
        f"-e {model_type} " \
        f"-s {search} " \
        f"--time-limit {timeout} " \
        f"--seed {seed} " \
        f"--translator-output-file {aux_file} " \
        f"--plan-file {plan_file}"
  return cmd, aux_file

def fd_cmd(df, pf, model_type, m, search, seed, profile, timeout=TIMEOUT, aux_file=None, plan_file=None):
  if search == "gbbfs":
    search = "batch_eager_greedy"
  elif search == "gbfs":
    search = "eager_greedy"
  else:
    raise NotImplementedError

  description = f"fd_{pf.replace('.pddl','').replace('/','-')}_{search}_{os.path.basename(m).replace('.dt', '')}".replace('.', '')

  if aux_file is None:
    os.makedirs("sas_files", exist_ok=True)
    aux_file = f"sas_files/{description}.sas_file"

  if plan_file is None:
    os.makedirs("plans", exist_ok=True)
    plan_file = f"plans/{description}.plan"

  if model_type == "kernel-opt":
    model = load_kernel_model_and_setup(m, df, pf)
    model.write_model_data()
    model.write_representation_to_file()
    model_data = model.get_model_data_path()
    graph_data = model.get_graph_file_path()

    cmd = f"./../downward/fast-downward.py --search-time-limit {timeout} --sas-file {aux_file} --plan-file {plan_file} "+\
          f"{df} {pf} --search '{search}([kernel(model_data=\"{model_data}\", "+\
                                               f"graph_data=\"{graph_data}\""+\
                                               f")])'"
    if profile:
      import shutil
      shutil.copyfile(model_data, model_data+"-copy")
      shutil.copyfile(graph_data, graph_data+"-copy")
      print("Running the original command to get individual commands for profiling...")
      output = os.popen(f"export GOOSE={os.getcwd()} && {cmd}").readlines()
      translator_cmd = ""
      search_cmd = ""
      for line in output:
        if "INFO     translator command line string:" in line:
          translator_cmd = line.replace("INFO     translator command line string:", "").replace('\n', '')
          continue
        if "INFO     search command line string:" in line:
          search_cmd = line.replace("INFO     search command line string:", "")
          continue
      shutil.move(model_data+"-copy", model_data)
      shutil.move(graph_data+"-copy", graph_data)
      cmd = f"{translator_cmd} && {PROFILE_CMD_} {search_cmd}"
      print("Original command completed.")
  else:
    cmd = f"./../downward/fast-downward.py --search-time-limit {timeout} --sas-file {aux_file} --plan-file {plan_file} "+\
          f"{df} {pf} --search '{search}([goose(model_path=\"{m}\", "+\
                                              f"model_type=\"{model_type}\", "+\
                                              f"domain_file=\"{df}\", "+\
                                              f"instance_file=\"{pf}\""+\
                                              f")])'"
  return cmd, aux_file
