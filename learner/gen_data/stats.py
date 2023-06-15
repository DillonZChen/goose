import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import warnings
import argparse
import logging
import os
import re
import signal
import torch
import subprocess
import pickle
import datetime
import random

from typing import Dict, List, Optional, Tuple
from torch_geometric.data import DataLoader, Data
from representation import *
from tqdm.auto import tqdm, trange
from planning import get_strips_problem
from util.ipc_domain_info import GENERAL_COST_DOMAINS, UNIT_COST_DOMAINS, IPC_DOMAINS, IPCS

assert len(UNIT_COST_DOMAINS.intersection(GENERAL_COST_DOMAINS)) == 0
assert UNIT_COST_DOMAINS.union(GENERAL_COST_DOMAINS) == IPC_DOMAINS


def get_domain_from_instance_file(problem_pddl: str, task_type: str) -> str:
  assert "ipc-" in problem_pddl
  ipc_i = problem_pddl.find("ipc-")
  ipc = problem_pddl[ipc_i:ipc_i+8]
  domain = problem_pddl.replace(f"../benchmarks/{ipc}/domains/", "").split("/")[0]
  domain_file = f"../benchmarks/{ipc}/domains/{domain}/domain.pddl"
  if not os.path.exists(domain_file):
    instance_num = re.findall(r'\d+', os.path.basename(problem_pddl))[0]
    domain_file = domain_file.replace("domain.pddl", f"domains/domain-{instance_num}.pddl")
  if task_type == "df":
    domain_file = domain_file.replace(".pddl", "_del_free.pddl")
  return domain_file


def get_data_dir_path(representation: str, task: str) -> str:
  # parser = params["parser"]
  # directed = params["directed"]
  # directed_folder = "directed" if directed else "undirected"
  save_dir = f'data/graphs/{representation}'
  os.makedirs(save_dir, exist_ok=True)
  return save_dir


# def get_data_path(domain_name: str, representation: str, task: str) -> str:
#   """ Get path to save file of graph training data of given domain. """
#   save_dir = get_data_dir_path(representation, task)
#   save_file = f'{save_dir}/{domain_name}_train.data'
#   return save_file


def get_data_path(domain_name: str,
                  domain_pddl: str,
                  problem_pddl: str,
                  representation: str,
                  task: str) -> str:
  """ Get path to save file of graph training data of given domain. """
  problem_name = os.path.basename(problem_pddl).replace(".pddl", "")
  save_dir = f'{get_data_dir_path(representation, task)}/{domain_name}'
  save_file = f'{save_dir}/{problem_name}.data'
  os.makedirs(save_dir, exist_ok=True)
  return save_file


def get_all_instance_files(del_free: bool = False, get_domain_name: bool=False) -> List[Tuple[str, str]]:
  ret = []
  for ipc in IPCS:
    dir_path = f'../benchmarks/{ipc}/domains'
    for domain_name in os.listdir(dir_path):
      if 'README' in domain_name:
        continue

      domain_pddl = f'{dir_path}/{domain_name}/domain.pddl'
      problem_path = f'{dir_path}/{domain_name}/instances'
      problem_pddls = [f'{problem_path}/{p_file}' for p_file in os.listdir(problem_path)]
      if os.path.exists(domain_pddl):
        for problem_pddl in problem_pddls:
          if get_domain_name:
            ret.append((f"{ipc}-{domain_name}", domain_pddl, problem_pddl))
          else:
            ret.append((domain_pddl, problem_pddl))
      else:
        for problem_pddl in problem_pddls:
          num = re.findall(r'\d+', os.path.basename(problem_pddl))[0]
          if del_free:
            domain_pddl = f'{dir_path}/{domain_name}/domains/domain-{num}_del_free.pddl'
          else:
            domain_pddl = f'{dir_path}/{domain_name}/domains/domain-{num}.pddl'
          assert os.path.exists(domain_pddl)
          assert os.path.exists(problem_pddl)
          if get_domain_name:
            ret.append((f"{ipc}-{domain_name}", domain_pddl, problem_pddl))
          else:
            ret.append((domain_pddl, problem_pddl))
  return ret


def optimal_plan_exists(domain_name: str, domain_pddl: str, problem_pddl: str, del_free: bool, parser: str):
  if parser=="powerlifted":
    suffix = "states"
    domain_name = domain_name.replace("htg-", '')
  else:
    suffix = "pkl"
  problem_name = os.path.basename(problem_pddl)
  del_free_suffix = 'df' if del_free else 'opt'
  save_dir = f'data/plan_objects/{parser}/{del_free_suffix}/{domain_name}'
  save_path = f'{save_dir}/{problem_name}.{suffix}'.replace(".pddl", "")
  if os.path.exists(save_path):  # if plan found, load and return
    if parser=="powerlifted":
      data = []
      lines = open(save_path, 'r').readlines()
      plan_length = len(lines)-1
      for i, line in enumerate(lines):
        if line[0]==";":
          assert "GOOD" in line
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
      return data
    else:
      file = open(save_path, 'rb')
      data = pickle.load(file)
      file.close()
    return data
  else:
    return None


def gen_del_free_domain(domain_pddl: str, domain_path: str, num=None):
  text = ""
  for line in open(domain_pddl, 'r').readlines():
    line = line.split(";", 1)[0]  # Strip comments.
    line = line.replace("(", " ( ").replace(")", " ) ").replace("?", " ?")
    text += line
  tokens = text.split()

  ret = ''
  found_not = False
  stack = 0
  i = 0
  while i < len(tokens) - 1:
    tok = tokens[i]
    if found_not:
      if tok == "(":
        stack += 1
      elif tok == ")":
        stack -= 1
        if stack == 0:
          found_not = False
          i += 1  # skip over next bracket
    elif tok == "not":
      found_not = True
    else:
      if not (tok == "(" and tokens[i + 1] == "not"):
        ret += tok + " "
    i += 1
  ret += tokens[-1]

  assert stack == 0
  assert not found_not

  if num is None:
    fname = f'{domain_path}/domain_del_free.pddl'
  else:
    fname = f'{domain_path}/domains/domain-{num}_del_free.pddl'
  # print(fname)
  file = open(fname, 'w')
  file.write(ret)
  file.close()
  return


def gen_del_free_domains():
  """ Create domain_del_free.pddl files for each domain """
  for ipc in IPCS:
    dir_path = f'../benchmarks/{ipc}/domains'
    for domain_name in os.listdir(dir_path):
      if 'README' in domain_name:
        continue
      domain_path = f'{dir_path}/{domain_name}'
      domain_pddl = f'{domain_path}/domain.pddl'
      if os.path.exists(domain_pddl):  # remove (not (...)) effects
        gen_del_free_domain(domain_path=domain_path, domain_pddl=domain_pddl)
      else:
        nums = set()
        for p_file in os.listdir(f"{domain_path}/instances"):
          nums.add(re.findall(r'\d+', p_file)[0])
        for num in nums:
          domain_pddl = f'{dir_path}/{domain_name}/domains/domain-{num}.pddl'
          gen_del_free_domain(domain_path=domain_path, domain_pddl=domain_pddl, num=num)
  return


# def test_plans():  # TODO: can be converted to compute statistics
#   """ Test optimal plan length >= relaxed plan length for all solved tasks """
#   cnt = 0
#   for ipc in IPCS:
#     for domain_name in os.listdir(f'../benchmarks/{ipc}/domains'):
#       if 'README' in domain_name:
#         continue
#
#       opt_plans = {}
#       del_free_plans = {}
#
#       try:
#         path = f'data/{plan_objects_path}/del_free/{ipc}-{domain_name}'
#         for problem_name in os.listdir(path):
#           file = open(f'{path}/{problem_name}', 'rb')
#           data = pickle.load(file)
#           file.close()
#           del_free_plans[problem_name] = data
#
#         path = f'data/{plan_objects_path}/opt/{ipc}-{domain_name}'
#         for problem_name in os.listdir(path):
#           file = open(f'{path}/{problem_name}', 'rb')
#           data = pickle.load(file)
#           file.close()
#           opt_plans[problem_name] = data
#       except:
#         continue
#
#       for prob in opt_plans:
#         if prob not in del_free_plans:
#           continue
#         opt_plan = opt_plans[prob]
#         del_free_plan = del_free_plans[prob]
#         if opt_plan is None or del_free_plan is None:
#           continue
#         difference = len(opt_plan) - len(del_free_plan)
#         assert (difference >= 0)
#         # print(difference)
#         cnt += 1
#   print(f"Delete relaxed plans look good! {cnt} tasks checked.")


def main():
  all_instance_files = get_all_instance_files(del_free=False)
  print(f"num instance files: {len(all_instance_files)}")
