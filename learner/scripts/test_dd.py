import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import argparse
import numpy as np

from dataset.goose_domain_info import GOOSE_DOMAINS
from representation import REPRESENTATIONS, CONFIG
from util.scrape_log import scrape_pwl_log, scrape_train_log
from util.search import *


FAIL_LIMIT = 6

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

  test_log_dir = f"logs/test"
  os.makedirs("logs", exist_ok=True)
  os.makedirs(test_log_dir, exist_ok=True)

  for domain in GOOSE_DOMAINS:
    test_dir = f"../benchmarks/goose/{domain}/test"
    for repeat in range(REPEATS):

      failed = 0  # skip experiments when fail FAIL_LIMIT times in a row
      model_file = f"dd_{rep}_{domain}_L{L}_H{H}_p{patience}_r{repeat}"
      df = f"../benchmarks/goose/{domain}/domain.pddl"

      # warmup the gpu (first problem always slower)
      f = sorted_nicely(os.listdir(test_dir))[0]
      pf = f"{test_dir}/{f}"
      cmd,lifted_file = search_cmd(rep, domain, df, pf, f"validated_models/{model_file}", "gbbfs", 0, timeout=30)
      os.system("date")
      print(f"warming up with {domain} {rep} {f.replace('.pddl', '')}")
      print(cmd)
      os.system(cmd)
      os.remove(lifted_file)

      # test
      for f in sorted_nicely(os.listdir(test_dir)):
        test_log_file = f"{test_log_dir}/{f.replace('.pddl', '')}_{model_file}.log"
        finished_correctly = False
        if os.path.exists(test_log_file):
          log = open(test_log_file, 'r').read()
          finished_correctly = "timed out after" in log or "Solution found." in log
        if not finished_correctly:
          pf = f"{test_dir}/{f}"
          cmd,lifted_file = search_cmd(rep, domain, df, pf, f"validated_models/{model_file}", "gbbfs", 0)
          os.system("date")
          print(f"testing {domain} {rep} {f.replace('.pddl', '')}")
          print(cmd)
          os.system(f"{cmd} > {test_log_file}")
          os.remove(lifted_file)

        # check if failed or not
        assert os.path.exists(test_log_file)
        log = open(test_log_file, 'r').read()
        solved = "Solution found." in log and "Iteration finished correctly." in log
        if solved:
          failed = 0
        else:
          failed += 1
        if failed >= FAIL_LIMIT:
          break
      # end f in [test problems]

  return


if __name__ == "__main__":
  main()
