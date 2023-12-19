import os
import sys

from sklearn.metrics import ConfusionMatrixDisplay, confusion_matrix

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import re
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from IPython.display import display, HTML
from representation import REPRESENTATIONS
from dataset.goose_domain_info import GOOSE_DOMAINS
from util.scrape_log import *
from pathlib import Path

""" Methods for constructing plots and visualising stats. """


FIRST = r"\first"
SECOND = r"\second"
THIRD = r"\third"
CELL = r"\normalcell"
ZERO = r"\zerocell"

CONFIG_TO_TEX = {
    "blind": r"\blind",
    "hff": r"\hfftable",
    "hff-pwl": r"\hffpwl",
    "shgn": r"\shgn",
    "lama-first": r"\lama",
    "ddg-el dd": r"\dlg\dd",
    "ddg-el di": r"\dlg\di",
    "fdg-el dd": r"\flg",
    "sdg-el dd": r"\slg",
    "ldg-el dd": r"\llg",
    "fdg-el di": r"\flg",
    "sdg-el di": r"\slg",
    "ldg-el di": r"\llg",
}

CONFIG_TO_PURE_TEX = {
    "ddg-el": r"$\textsf{DLG}$",
    "fdg-el": r"$\textsf{FLG}$",
    "sdg-el": r"$\textsf{SLG}$",
    "ldg-el": r"$\textsf{LLG}$",
}

CONFIG_TO_LINE_STYLE = {
    "blind": "solid",
    "hff": "solid",
    "hff-pwl": "solid",
    "shgn": "solid",
    "ddg-el dd": "solid",
    "ddg-el di": "solid",
    "lama-first": "solid",
    "fdg-el dd": "dashed",
    "sdg-el dd": "dashed",
    "ldg-el dd": "dashed",
    "fdg-el di": "dotted",
    "sdg-el di": "dotted",
    "ldg-el di": "dotted",
}

CONFIG_TO_COLOUR = {
    "blind": "black",
    "hff": "orange",
    "hff-pwl": "brown",
    "shgn": "green",
    "ddg-el dd": "green",
    "ddg-el di": "olive",
    "lama-first": "green",
    "fdg-el dd": "red",
    "sdg-el dd": "purple",
    "ldg-el dd": "blue",
    "fdg-el di": "red",
    "sdg-el di": "purple",
    "ldg-el di": "blue",
}

GOOSE_DOMAINS = sorted(GOOSE_DOMAINS)


def sorted_nicely( l ): 
  """ Sort the given iterable in the way that humans expect.""" 
  convert = lambda text: int(text) if text.isdigit() else text 
  alphanum_key = lambda key: [ convert(c) for c in re.split('([0-9]+)', key) ] 
  return sorted(l, key = alphanum_key)

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
          problems = os.listdir(f"../dataset/goose/{domain}/test")
          for problem_pddl in problems:
            problem_name = os.path.basename(problem_pddl).replace(".pddl", "")
            f = f'{log_dir}/{problem_name}_{train_type}_{rep}_{domain}_L{L}_H{H}_{aggr}_p{p}_r0.log'
            if not os.path.exists(f):
              continue
            problem_stats = scrape_search_log(f)
        
            if normalise:
              solved += float(problem_stats["solved"]) / float(len(problems))
            else:
              solved += int(problem_stats["solved"])


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

def collect_test_stats_planner_and_graphs(configs, L, aggr, normalise):
  d = {
    "config": [],
    "solved": [],
    "expanded": [],
    "time": [],
    "cost": [],
    "domain": [],
  }

  for domain in GOOSE_DOMAINS:
    problems = os.listdir(f"../dataset/goose/{domain}/test")
    for problem_pddl in problems:
      problem_name = os.path.basename(problem_pddl).replace(".pddl", "")

      for config in configs:
        if "-el" in config:
          # graph
          rep, train_type = config.split()
          log_dir = "logs/test"

          H=64
          p=10 if train_type=="dd" else 20

          f = f'{log_dir}/{problem_name}_{train_type}_{rep}_{domain}_L{L}_H{H}_{aggr}_p{p}_r0.log'
          tmp = scrape_search_log(f)
        else:
          # planner
          log_dir = f"logs/{config}"
          if config in {"shgn", "hff-pwl"}:
            f = f'{log_dir}/{domain}_{problem_name}.log'
          else:
            f = f'{log_dir}/{domain}_{problem_name}_{config}.log'
          # assert os.path.exists(f)
          tmp = scrape_search_log(f)

        tmp["config"] = config
        tmp["domain"] = domain

        for key in d:
          val = tmp[key]
          if normalise and key=="solved":
            val = float(val) / float(len(problems))
          d[key].append(val)
  df = pd.DataFrame(d)
  return df


def collect_test_stats_planner(planner, normalise, domain=None):
  d = {
    "solved": [],
    "expanded": [],
    "time": [],
    "cost": [],
  }
  log_dir = f"logs/{planner}"

  if domain is None:
    domains = GOOSE_DOMAINS
  else:
    domains = [domain]
  for domain in domains:
    problem_pddls = os.listdir(f"../dataset/goose/{domain}/test")
    for problem_pddl in problem_pddls:
      problem_name = os.path.basename(problem_pddl).replace(".pddl", "")
      tmp = {}
      f = f'{log_dir}/{domain}_{problem_name}_{planner}.log'
      assert os.path.exists(f)
      problem_stats = scrape_search_log(f)
      for k in d:
        if k in problem_stats:
          tmp[k] = problem_stats[k]
          if k=="solved" and normalise:
            tmp[k] = float(tmp[k])/len(problem_pddls)
      assert len(tmp) == len(d)
      for k in d:
        d[k].append(tmp[k])

  df = pd.DataFrame(d)
  if not normalise:
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
      for problem_pddl in os.listdir(f"../dataset/goose/{domain}/test"):
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

def get_confusion_matrix(y_true_train, y_pred_train, y_true_test, y_pred_test, cutoff=-1):
  y_true_train = np.rint(y_true_train).astype(int)
  y_pred_train = np.rint(y_pred_train).astype(int)
  y_true_test = np.rint(y_true_test).astype(int)
  y_pred_test = np.rint(y_pred_test).astype(int)
  fig, ax = plt.subplots(nrows=1, ncols=2, figsize=(10, 10))
  if cutoff == -1:
    cutoff = max(max(y_true_train), max(y_true_test))+1
  cm_train = confusion_matrix(y_true_train, y_pred_train, normalize="true", labels=list(range(0, cutoff)))
  cm_test = confusion_matrix(y_true_test, y_pred_test, normalize="true", labels=list(range(0, cutoff)))
  display_labels = [y if y%10==0 else "" for y in range(cutoff)]
  for i, cm in enumerate([cm_train, cm_test]):
    disp = ConfusionMatrixDisplay(confusion_matrix=cm, display_labels=display_labels)
    disp.plot(include_values=False, xticks_rotation="vertical", ax=ax[i], colorbar=False)
    disp.im_.set_clim(0, 1)
  return plt
