import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import numpy as np
import plotly.express as px
import matplotlib.pyplot as plt
import pandas as pd

from IPython.display import display, HTML
from representation import REPRESENTATIONS
from dataset import GOOSE_DOMAINS
from util.scrape_log import *


def collect_param_test_stats(train_type, Ls, aggrs, H, p, graphs, normalise, domain=None):
  d = {"aggr":[],"L":[],}
  for graph in graphs:
    d[graph] = []
  log_dir = "logs/test"
  domains = [domain] if domain != None else GOOSE_DOMAINS
  for aggr in aggrs:
    for L in Ls:
      d["L"].append(L)
      d["aggr"].append(aggr)
      for rep in graphs:
        solved = 0
        for domain in domains:
          problems = os.listdir(f"../benchmarks/goose/{domain}/test")
          for problem_pddl in problems:
            problem_name = os.path.basename(problem_pddl).replace(".pddl", "")
            f = f'{log_dir}/{problem_name}_{train_type}_{rep}_{domain}_L{L}_H{H}_{aggr}_p{p}_r0.log'
            if not os.path.exists(f):
              continue
            problem_stats = scrape_search_log(f)
            solved += int(problem_stats["solved"])
        
        if normalise:
          solved = float(solved) / float(len(problems))
        d[rep].append(solved)

  df = pd.DataFrame(d)

  return df


def collect_val_stats(train_type, L, H, aggr, p):
  # Collect stats from the logs/select directory
  # TODO multiple repeats

  d = {
    "graph": [],
    "domain": [],
    "L": [],
    "H": [],
    "aggr": [],
    "patience": [],
    "solved": [],
    "median_expansions": [],
    "median_runtime": [],
    "avg_loss": [],
    "train_time": [],
  }
  log_dir = "logs/select"
  for rep in REPRESENTATIONS:
    for domain in GOOSE_DOMAINS:
      try:
        tmp = {}
        tmp["graph"] = rep
        tmp["domain"] = domain
        tmp["L"] = L
        tmp["H"] = H
        tmp["aggr"] = aggr
        tmp["patience"] = p
        f = f'{log_dir}/{train_type}_{rep}_{domain}_L{L}_H{H}_{aggr}_p{p}_r0.log'
        for line in open(f, 'r').readlines():
          toks = line.replace(":","").split()
          for key in d:
            if key==toks[0]:
              val = int(float(toks[1])) if key in key in {"solved", "median_expansions"} else float(toks[1])
              tmp[key] = val
              break
        for key in d:
          d[key].append(tmp[key])
      except:
        pass

  df = pd.DataFrame(d)

  return df

def display_val_stats(train_type, L, H, aggr, p):
  # Display stats from the logs/select directory
  # TODO multiple repeats

  df = collect_val_stats(train_type, L, H, aggr, p)
  for domain in GOOSE_DOMAINS:
    df_domain = df[df.domain==domain]
    display(df_domain)

  return

def collect_test_stats_planner(planner):
  d = {
    "domain": [],
    "solved": [],
    "expanded": [],
    "time": [],
    "cost": [],
  }
  log_dir = f"logs/{planner}"

  for domain in GOOSE_DOMAINS:
    for problem_pddl in os.listdir(f"../benchmarks/goose/{domain}/test"):
      problem_name = os.path.basename(problem_pddl).replace(".pddl", "")
      tmp = {}
      tmp["domain"] = domain
      f = f'{log_dir}/{domain}_{problem_name}_{planner}.log'
      assert os.path.exists(f)
      problem_stats = scrape_search_log(f)
      for k in d:
        if k in problem_stats:
          tmp[k] = problem_stats[k]
      assert len(tmp) == len(d)
      for k in d:
        d[k].append(tmp[k])

  df = pd.DataFrame(d)
  df["solved"] = df["solved"].astype(int)
  return df

def collect_test_stats(train_type, L, H, aggr, p):
  # Collect stats from the logs/test directory
  # TODO multiple repeats

  d = {
    "graph": [],
    "domain": [],
    "L": [],
    "H": [],
    "aggr": [],
    "patience": [],
    "solved": [],
    "expanded": [],
    "time": [],
    "cost": [],
  }
  log_dir = "logs/test"
  for rep in REPRESENTATIONS:
    for domain in GOOSE_DOMAINS:
      for problem_pddl in os.listdir(f"../benchmarks/goose/{domain}/test"):
        problem_name = os.path.basename(problem_pddl).replace(".pddl", "")
        tmp = {}
        tmp["graph"] = rep
        tmp["domain"] = domain
        tmp["L"] = L
        tmp["H"] = H
        tmp["aggr"] = aggr
        tmp["patience"] = p
        f = f'{log_dir}/{problem_name}_{train_type}_{rep}_{domain}_L{L}_H{H}_{aggr}_p{p}_r0.log'
        if not os.path.exists(f):
          continue
        problem_stats = scrape_search_log(f)
        for k in d:
          if k in problem_stats:
            tmp[k] = problem_stats[k]
        if len(tmp) != len(d):
          continue
        for k in d:
          d[k].append(tmp[k])

  df = pd.DataFrame(d)
  df["solved"] = df["solved"].astype(int)

  return df

def display_solved_test_stats(train_type, L, H, aggr, p):
  # Display solved stats from the logs/test directory
  # TODO multiple repeats

  df = collect_test_stats(train_type, L, H, aggr, p)
  for domain in GOOSE_DOMAINS:
    df_domain = df[df.domain==domain]
    df_rep = df_domain.drop(columns=["domain", "L", "H", "aggr", "patience"]).groupby("graph").sum()
    print(domain)
    display(df_rep)

  return

def get_max_of_parameters(df):
  df = df.drop(columns=["L", "aggr"]).max()
  return df
