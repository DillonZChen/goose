import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import argparse
import numpy as np

from dataset.goose_domain_info import GOOSE_DOMAINS
from representation import REPRESENTATIONS, CONFIG
from util.scrape_log import scrape_pwl_log, scrape_train_log

REPEATS = 1
VAL_REPEATS = 5

L=8
H=64
patience=10


def pwl_cmd(domain_name, df, pf, m, search, seed, timeout=120):
  os.makedirs("lifted", exist_ok=True)
  os.makedirs("plan", exist_ok=True)
  description = f"{domain_name}_{os.path.basename(pf).replace('.pddl','')}_{search}_{os.path.basename(m).replace('.dt', '')}"
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


def main():
  parser=argparse.ArgumentParser()
  parser.add_argument("rep", type=str, choices=REPRESENTATIONS)
  args = parser.parse_args()
  rep = args.rep

  train_log_dir = f"logs/train"
  val_log_dir = f"logs/val"
  selection_log_dir = f"logs/select"
  os.makedirs("logs", exist_ok=True)
  os.makedirs(train_log_dir, exist_ok=True)
  os.makedirs(val_log_dir, exist_ok=True)
  os.makedirs(selection_log_dir, exist_ok=True)
  os.makedirs("validated_models", exist_ok=True)

  for domain in GOOSE_DOMAINS:
    val_dir = f"../benchmarks/goose/{domain}/val"
    for repeat in range(REPEATS):

      # for each experiment, we have validation repeats
      for val_repeat in range(VAL_REPEATS):
        model = "RGNN" if CONFIG[rep]['edge_labels'] else "MPNN"
        model_file = f"dd_{rep}_{domain}_L{L}_H{H}_p{patience}_v{val_repeat}_r{repeat}"
        
        # train
        if not os.path.exists(f"trained_models/{model_file}.dt"):
          train_log_file = f"{train_log_dir}/{model_file}.log"
          cmd = f"python3 train.py --fast-train --no-tqdm -r {rep} -m {model} -d goose-{domain}-only -L {L} -H {H} --patience {patience} --save-file {model_file}"
          os.system("date")
          print("training")
          print(cmd)
          os.system(f"{cmd} > {train_log_file}")

        # validate
        df = f"../benchmarks/goose/{domain}/domain.pddl"
        for f in os.listdir(val_dir):
          val_log_file = f"{val_log_dir}/{f.replace('.pddl', '')}_{model_file}.log"
          finished_correctly = False
          if os.path.exists(val_log_file):
            log = open(val_log_file, 'r').read()
            finished_correctly = "timed out after" in log or "Solution found." in log
          if not finished_correctly:
            pf = f"{val_dir}/{f}"
            cmd,lifted_file = pwl_cmd(domain, df, pf, f"trained_models/{model_file}", "gbbfs", 0)
            os.system("date")
            print("validating")
            print(cmd)
            os.system(f"{cmd} > {val_log_file}")
            os.remove(lifted_file)

      # after running all validation repeats, we pick the best one
      best_model = -1
      best_solved = 0
      best_expansions = float('inf')
      best_runtimes = float('inf')
      for val_repeat in range(VAL_REPEATS):
        model_file = f"dd_{rep}_{domain}_L{L}_H{H}_p{patience}_v{val_repeat}_r{repeat}"
        solved = 0
        expansions = []
        runtimes = []
        for f in os.listdir(val_dir):
          val_log_file = f"{val_log_dir}/{f.replace('.pddl', '')}_{model_file}.log"
          stats = scrape_pwl_log(val_log_file)
          solved += stats["solved"]
          expansions.append(stats["expanded"])
          runtimes.append(stats["time"])
        expansions = np.median(expansions) 
        runtimes = np.median(runtimes)
        if solved >= best_solved and expansions < best_expansions:
          best_model = model_file
          best_solved = solved
          best_expansions = expansions
          best_runtimes = runtimes
      best_model_file = f"dd_{rep}_{domain}_L{L}_H{H}_p{patience}_r{repeat}"
      train_stats = scrape_train_log(f"{train_log_dir}/{best_model}.log")

      with open(f"{selection_log_dir}/{best_model_file}.log", 'w') as f:
        f.write(f"model: {best_model}\n")
        f.write(f"solved: {best_solved} / {len(os.listdir(val_dir))}\n")
        f.write(f"median_expansions: {best_expansions}\n")
        f.write(f"median_runtime: {best_runtimes}\n")
        f.write(f"avg_loss: {train_stats['best_avg_loss']}\n")
        f.write(f"train_time: {train_stats['time']}\n")
        f.close()
      os.system(f"cp trained_models/{best_model}.dt validated_models/{best_model_file}.dt")

  return

if __name__ == "__main__":
  main()
