import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import argparse
import numpy as np

from dataset.goose_domain_info import GOOSE_DOMAINS
from representation import REPRESENTATIONS, CONFIG
from util.scrape_log import scrape_pwl_log, scrape_train_log
from util.search import *


def main():
  parser=argparse.ArgumentParser()
  parser.add_argument("rep", type=str, choices=REPRESENTATIONS)
  parser.add_argument("-L", type=int, default=16)
  parser.add_argument("-H", type=int, default=64)
  parser.add_argument("-p", type=int, default=10)
  args = parser.parse_args()
  rep = args.rep
  L = args.L
  H = args.H
  patience = args.p

  train_log_dir = f"logs/train"
  val_log_dir = f"logs/val"
  selection_log_dir = f"logs/select"
  test_log_dir = f"logs/test"
  os.makedirs("logs", exist_ok=True)
  os.makedirs(train_log_dir, exist_ok=True)
  os.makedirs(val_log_dir, exist_ok=True)
  os.makedirs(selection_log_dir, exist_ok=True)
  os.makedirs(test_log_dir, exist_ok=True)
  os.makedirs("validated_models", exist_ok=True)

  for domain in GOOSE_DOMAINS:
    val_dir = f"../benchmarks/goose/{domain}/val"
    for repeat in range(REPEATS):

      # for each experiment, we have validation repeats
      for val_repeat in range(VAL_REPEATS):
        model = "RGNN" if CONFIG[rep]['edge_labels'] else "MPNN"
        model_file = f"dd_{rep}_{domain}_L{L}_H{H}_p{patience}_v{val_repeat}_r{repeat}"
        
        """ train """
        if not os.path.exists(f"trained_models/{model_file}.dt"):
          train_log_file = f"{train_log_dir}/{model_file}.log"
          cmd = f"python3 train.py --fast-train --no-tqdm -r {rep} -m {model} -d goose-{domain}-only -L {L} -H {H} --patience {patience} --save-file {model_file}"
          os.system("date")
          print("training")
          print(cmd)
          os.system(f"{cmd} > {train_log_file}")

        """ validate """
        df = f"../benchmarks/goose/{domain}/domain.pddl"
        for f in os.listdir(val_dir):
          val_log_file = f"{val_log_dir}/{f.replace('.pddl', '')}_{model_file}.log"
          finished_correctly = False
          if os.path.exists(val_log_file):
            log = open(val_log_file, 'r').read()
            finished_correctly = "timed out after" in log or "Solution found." in log
          if not finished_correctly:
            pf = f"{val_dir}/{f}"
            cmd,intermediate_file = pwl_cmd(domain, df, pf, f"trained_models/{model_file}", "gbbfs", 0)
            os.system("date")
            print("validating")
            print(cmd)
            os.system(f"{cmd} > {val_log_file}")
            try:
                os.remove(intermediate_file)
            except OSError:
                pass

      # after running all validation repeats, we pick the best one
      best_model = -1
      best_solved = 0
      best_expansions = float('inf')
      best_runtimes = float('inf')
      best_loss = float('inf')
      best_train_time = float('inf')

      # see if any model solved anything
      for val_repeat in range(VAL_REPEATS):
        model_file = f"dd_{rep}_{domain}_L{L}_H{H}_p{patience}_v{val_repeat}_r{repeat}"
        solved = 0
        for f in os.listdir(val_dir):
          val_log_file = f"{val_log_dir}/{f.replace('.pddl', '')}_{model_file}.log"
          stats = scrape_pwl_log(val_log_file)
          solved += stats["solved"]
        best_solved = max(best_solved, solved)

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
        train_stats = scrape_train_log(f"{train_log_dir}/{model_file}.log")
        avg_loss = train_stats['best_avg_loss']
        train_time = train_stats['time']
        # choose best model
        if (solved==best_solved and best_solved>0 and expansions<best_expansions) or \
           (solved==best_solved and best_solved==0 and avg_loss<best_loss):
          best_model = model_file
          best_expansions = expansions
          best_runtimes = runtimes
          best_loss = avg_loss
          best_train_time = train_time

      # log best model stats
      best_model_file = f"dd_{rep}_{domain}_L{L}_H{H}_p{patience}_r{repeat}"
      with open(f"{selection_log_dir}/{best_model_file}.log", 'w') as f:
        f.write(f"model: {best_model}\n")
        f.write(f"solved: {best_solved} / {len(os.listdir(val_dir))}\n")
        f.write(f"median_expansions: {best_expansions}\n")
        f.write(f"median_runtime: {best_runtimes}\n")
        f.write(f"avg_loss: {best_loss}\n")
        f.write(f"train_time: {best_train_time}\n")
        f.close()
      os.system(f"cp trained_models/{best_model}.dt validated_models/{best_model_file}.dt")
      ##### end validate code #####

    """ test """
    failed = 0
    test_dir = f"../benchmarks/goose/{domain}/test"
    df = f"../benchmarks/goose/{domain}/domain.pddl"
    model_file = best_model_file

    # warmup first
    f = sorted_nicely(os.listdir(test_dir))[0]
    pf = f"{test_dir}/{f}"
    cmd,intermediate_file = search_cmd(rep, domain, df, pf, f"validated_models/{model_file}", "gbbfs", 0)
    os.system("date")
    print(f"warming up with {domain} {rep} {f.replace('.pddl', '')}")
    print(cmd)
    os.system(cmd)
    try:
        os.remove(intermediate_file)
    except OSError:
        pass

    for f in sorted_nicely(os.listdir(test_dir)):
      test_log_file = f"{test_log_dir}/{f.replace('.pddl', '')}_{model_file}.log"
      finished_correctly = False
      if os.path.exists(test_log_file):
        log = open(test_log_file, 'r').read()
        finished_correctly = "timed out after" in log or "Solution found." in log
      if not finished_correctly:
        pf = f"{test_dir}/{f}"
        cmd,intermediate_file = search_cmd(rep, domain, df, pf, f"validated_models/{model_file}", "gbbfs", 0)
        os.system("date")
        print(f"testing {domain} {rep} {f.replace('.pddl', '')}")
        print(cmd)
        os.system(f"{cmd} > {test_log_file}")
        try:
            os.remove(intermediate_file)
        except OSError:
            pass


      # check if failed or not
      assert os.path.exists(test_log_file)
      log = open(test_log_file, 'r').read()
      solved = "Solution found." in log and "Iteration finished correctly." in log
      if solved:
        failed = 0
      else:
        failed += 1
      if failed >= FAIL_LIMIT[domain]:
        break
    # end f in [test problems]
  return


if __name__ == "__main__":
  main()
