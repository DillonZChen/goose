""" File for generating and loading graphs for kernels. Used by scripts/generate_graphs_kernel.py """

import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import torch
import dataset.ipc_domain_info as ipc_domain_info
import dataset.htg_domain_info as htg_domain_info
import dataset.goose_domain_info as goose_domain_info

from tqdm import tqdm, trange
from typing import Dict, List, Optional, Tuple
from representation import REPRESENTATIONS
from dataset.htg_domain_info import get_all_htg_instance_files
from dataset.ipc_domain_info import same_domain, GROUNDED_DOMAINS, get_ipc_domain_problem_files
from dataset.goose_domain_info import get_train_goose_instance_files
from dataset.ipc2023_learning_domain_info import get_train_ipc2023_learning_instance_files
from representation import CGraph


_SAVE_DIR = "data/graphs_kernel"
Data = Tuple[CGraph, int]

def generate_graph_from_domain_problem_pddl(
  domain_name: str,
  domain_pddl: str,
  problem_pddl: str,
  representation: str,
) -> Optional[List[Data]]:
  """ Generates a list of graphs corresponding to states in the optimal plan """
  ret = []

  plan = optimal_plan_exists(domain_name, domain_pddl, problem_pddl)
  if plan is None:
    return None
  
  # see representation package
  rep = REPRESENTATIONS[representation](domain_pddl, problem_pddl)
  rep.convert_to_coloured_graph()

  problem_name = os.path.basename(problem_pddl).replace(".pddl", "")

  for s, y, a in plan:
    if REPRESENTATIONS[representation].lifted:
      s = rep.str_to_state(s)

    graph = rep.state_to_cgraph(s)
    ret.append((graph, y))
  return ret

def get_graph_data(
  representation: str,
  domain: str="all",
) -> List[Data]:
  """ Load stored generated graphs """

  print("Loading train data...")
  print("NOTE: the data has been precomputed and saved.")
  print("Exec 'python3 scripts_kernel/generate_graphs_kernel.py --regenerate' if representation has been updated!")

  path = get_data_dir_path(representation=representation)
  print(f"Path to data: {path}")

  ret = []
      
  for data in sorted(list(os.listdir(f"{path}/{domain}"))):
    next_data = torch.load(f'{path}/{domain}/{data}')
    ret+=next_data

  print(f"{domain} dataset of size {len(ret)} loaded!")
  return ret

def generate_graph_rep_domain(
  domain_name: str,
  domain_pddl: str,
  problem_pddl: str,
  representation: str,
  regenerate: bool
) -> int:
  """ Saves list of torch_geometric.data.Data of graphs and features to file. 
      Returns a new graph was generated or not
  """
  save_file = get_data_path(domain_name,
                            domain_pddl,
                            problem_pddl,
                            representation)
  if os.path.exists(save_file):
    if not regenerate:
      return 0
    else:
      os.remove(save_file)  # make a fresh set of data
  
  graph = generate_graph_from_domain_problem_pddl(domain_name=domain_name,
                                                  domain_pddl=domain_pddl,
                                                  problem_pddl=problem_pddl,
                                                  representation=representation)
  if graph is not None:
    tqdm.write(f'saving data @{save_file}...')
    torch.save(graph, save_file)
    tqdm.write('data saved!')
    return 1
  return 0

def gen_graph_rep(
  representation: str,
  regenerate: bool,
  domain: str,
) -> None:
  """ Generate graph representations from saved optimal plans. """

  tasks = get_train_goose_instance_files()
  tasks += get_train_ipc2023_learning_instance_files()

  # // TODO remove
  # tasks = get_train_ipc2023_learning_instance_files()
  # // TODO remove

  new_generated = 0
  pbar = tqdm(tasks)
  for domain_name, domain_pddl, problem_pddl in tasks:
    problem_name = os.path.basename(problem_pddl).replace(".pddl", "")
    # if representation in LIFTED_REPRESENTATIONS and domain_name in GROUNDED_DOMAINS:
    #   continue
    pbar.set_description(f"Generating {representation} graphs for {domain_name} {problem_name}")

    # in case we only want to generate graphs for one specific domain
    if domain is not None and domain != domain_name:
      continue

    new_generated += generate_graph_rep_domain(domain_name=domain_name,
                                                domain_pddl=domain_pddl,
                                                problem_pddl=problem_pddl,
                                                representation=representation,
                                                regenerate=regenerate)
  print(f"newly generated graphs: {new_generated}")
  return

def get_data_dir_path(representation: str) -> str:
  save_dir = f'{_SAVE_DIR}/{representation}'
  os.makedirs(save_dir, exist_ok=True)
  return save_dir

def get_data_path(domain_name: str,
                  domain_pddl: str,
                  problem_pddl: str,
                  representation: str) -> str:
  """ Get path to save file of graph training data of given domain. """
  problem_name = os.path.basename(problem_pddl).replace(".pddl", "")
  save_dir = f'{get_data_dir_path(representation)}/{domain_name}'
  save_file = f'{save_dir}/{problem_name}.data'
  os.makedirs(save_dir, exist_ok=True)
  return save_file

def optimal_plan_exists(domain_name: str, domain_pddl: str, problem_pddl: str):
  domain_name = domain_name.replace("htg-", '')
  problem_name = os.path.basename(problem_pddl)
  save_dir = f'data/plan_objects/{domain_name}'
  save_path = f'{save_dir}/{problem_name}.states'.replace(".pddl", "")
  good = 0
  if os.path.exists(save_path):  # if plan found, load and return
    data = []
    lines = open(save_path, 'r').readlines()
    plan_length = len(lines)-1
    for i, line in enumerate(lines):
      if line[0]==";":
        assert "GOOD" in line
        good = True
      else:
        line = line.replace("\n", "")
        s = set()
        for fact in line.split():
          if "(" not in fact:
            lime = f"({fact})"
          else:
            pred = fact[:fact.index("(")]
            fact = fact.replace(pred+"(","").replace(")","")
            args = fact.split(",")[:-1]
            lime = f"({pred}"
            for j, arg in enumerate(args):
              lime+=f" {arg}"
              if j == len(args)-1:
                lime+=")"
          s.add(lime)
        y = plan_length - i - 1
        a = None
        data.append((s, y, a))
    if not good:
      print(f"plan was not good for {save_path}")
      # data/plan_objects/ipc2023-learning-childsnack/p26.states
      # os.system(f"touch {domain_name}_{problem_name}.states")
      return None
    return data
  else:
    return None
