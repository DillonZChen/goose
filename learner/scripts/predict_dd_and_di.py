import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import argparse
import numpy as np

from dataset.goose_domain_info import GOOSE_DOMAINS
from representation import REPRESENTATIONS
from util.scrape_log import scrape_search_log, scrape_train_log, predict_finished_correctly
from util.search import *


if __name__ == "__main__":
  parser=argparse.ArgumentParser()
  parser.add_argument("rep", type=str, choices=REPRESENTATIONS)
  parser.add_argument("-L", type=int)
  parser.add_argument("-H", type=int)
  parser.add_argument("-a", type=str)
  args = parser.parse_args()
  rep = args.rep
  L = args.L
  H = args.H
  aggr = args.a

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

  for train_type, patience in [("di", 20), ("dd", 10)]:

    # validate and test each domain
    for domain in GOOSE_DOMAINS:
      val_dir = f"../benchmarks/goose/{domain}/val"
      test_dir = f"../benchmarks/goose/{domain}/test"
      for repeat in range(REPEATS):

        # after running all validation repeats, we pick the best one
        best_model = -1
        best_solved = 0
        best_expansions = float('inf')
        best_runtimes = float('inf')
        best_loss = float('inf')
        best_train_time = float('inf')

        # see if any model solved anything
        for val_repeat in range(VAL_REPEATS):
          model_file = f"{train_type}_{rep}_L{L}_H{H}_{aggr}_p{patience}_v{val_repeat}_r{repeat}"
          solved = 0
          for f in os.listdir(val_dir):
            val_log_file = f"{val_log_dir}/{f.replace('.pddl', '')}_{model_file}.log"
            stats = scrape_search_log(val_log_file)
            solved += stats["solved"]
          best_solved = max(best_solved, solved)

        for val_repeat in range(VAL_REPEATS):
          model_file = f"{train_type}_{rep}_L{L}_H{H}_{aggr}_p{patience}_v{val_repeat}_r{repeat}"
          solved = 0
          expansions = []
          runtimes = []
          for f in os.listdir(val_dir):
            val_log_file = f"{val_log_dir}/{f.replace('.pddl', '')}_{model_file}.log"
            assert os.path.exists(val_log_file)
            stats = scrape_search_log(val_log_file)
            solved += stats["solved"]
            if stats["solved"]:
              expansions.append(stats["expanded"])
              runtimes.append(stats["time"])
          expansions = np.median(expansions) if len(expansions)>0 else -1
          runtimes = np.median(runtimes) if len(runtimes)>0 else -1
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
        best_model_file = f"{train_type}_{rep}_{domain}_L{L}_H{H}_{aggr}_p{patience}_r{repeat}"
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
        df = f"../benchmarks/goose/{domain}/domain.pddl"
        model_file = best_model_file

        # warmup first
        f = sorted_nicely(os.listdir(test_dir))[0]
        pf = f"{test_dir}/{f}"
        cmd,intermediate_file = search_cmd(rep, domain, df, pf, f"validated_models/{model_file}", "gbbfs", 0, timeout=30)
        os.system("date")
        os.system(f"echo warming up with {domain} {rep} {f.replace('.pddl', '')} {model_file}")
        os.popen(cmd).readlines()
        try:
            os.remove(intermediate_file)
        except OSError:
            pass

        # test on problems
        for f in sorted_nicely(os.listdir(test_dir)):
          os.system("date")
          test_log_file = f"{test_log_dir}/{f.replace('.pddl', '')}_{model_file}.log"
          finished_correctly = False
          if os.path.exists(test_log_file): 
            finished_correctly = predict_finished_correctly(test_log_file)
          if not finished_correctly:
            pf = f"{test_dir}/{f}"
            cmd,intermediate_file = search_cmd(rep, domain, df, pf, f"validated_models/{model_file}", "gbbfs", 0, timeout=20)
            os.system(f"echo predicting {domain} {rep}, see {test_log_file}")
            os.system(f"{cmd} > {test_log_file}")
            try:
                os.remove(intermediate_file)
            except OSError:
                pass
          else:
            os.system(f"echo already predicted for {domain} {rep}, see {test_log_file}")

          # replace "Time limit has been reached."
          assert os.path.exists(test_log_file)
          log = open(test_log_file, 'r').read()
          log = log.replace("timed out after", "")
          log = log.replace("Time limit has been reached.", "")
          f = open(test_log_file, 'w')
          f.write(log)
          f.close()

        # end f in [test problems]
