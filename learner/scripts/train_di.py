import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import argparse
import numpy as np

from dataset.goose_domain_info import GOOSE_DOMAINS
from representation import REPRESENTATIONS, CONFIG
from util.scrape_log import scrape_search_log, scrape_train_log
from util.search import *


def main():
  parser=argparse.ArgumentParser()
  parser.add_argument("rep", type=str, choices=REPRESENTATIONS)
  parser.add_argument("-L", type=int)
  parser.add_argument("-H", type=int)
  parser.add_argument("-a", type=str)
  parser.add_argument("-p", type=int)
  args = parser.parse_args()
  rep = args.rep
  L = args.L
  H = args.H
  aggr = args.a
  patience = args.p

  train_log_dir = f"logs/train"
  os.makedirs("logs", exist_ok=True)
  os.makedirs(train_log_dir, exist_ok=True)

  for repeat in range(REPEATS):

    # for each experiment, we have validation repeats
    for val_repeat in range(VAL_REPEATS):
      model = "RGNN" if CONFIG[rep]['edge_labels'] else "MPNN"
      model_file = f"di_{rep}_L{L}_H{H}_{aggr}_p{patience}_v{val_repeat}_r{repeat}"
      
      # train
      if not os.path.exists(f"trained_models/{model_file}.dt"):
        train_log_file = f"{train_log_dir}/{model_file}.log"
        cmd = f"python3 train.py --fast-train --no-tqdm -r {rep} -m {model} -d goose-unseen-pretraining -L {L} -H {H} --aggr {aggr} --patience {patience} --save-file {model_file} -n 10000"
        os.system("date")
        os.system(f"training domain independent {rep}")
        os.system(f"{cmd} > {train_log_file}")

  return


if __name__ == "__main__":
  main()
