import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from util.goose_domain_info import GOOSE_DOMAINS
from representation import CONFIG
import argparse

EXPERIMENT_REPRESENTATIONS = [
  "gdg-el",
  "sdg-el",
  "fdg-el",
  "ldg-el",
]

REPEATS = 1
VAL_REPEATS = 5

L=8
H=32
patience=10

parser=argparse.ArgumentParser()
parser.add_argument("domain", type=str, choices=GOOSE_DOMAINS)
parser.add_argument("--reverse", action='store_true')
args = parser.parse_args()
domain = args.domain

for repeat in range(REPEATS):
  log_dir = f"logs/train"
  os.makedirs(log_dir, exist_ok=True)

  REPEATS = range(VAL_REPEATS)
  if args.reverse:
    REPEATS = reversed(REPEATS)

  for val_repeat in REPEATS:
    for rep in EXPERIMENT_REPRESENTATIONS:
      model = "RGNN" if CONFIG[rep]['edge_labels'] else "MPNN"
      model_file = f"dd_{rep}_{domain}_L{L}_H{H}_p{patience}_v{val_repeat}_r{repeat}"
      log_file = f"{log_dir}/{model_file}.log"
      if os.path.exists(f"trained_models/{model_file}.dt") or os.path.exists(log_file):
        print(f"model exists for {model_file}")
        continue
      cmd = f"python3 train.py --fast-train --no-tqdm -r {rep} -m {model} -d goose-{domain}-only -L {L} -H {H} --patience {patience} --save-file {model_file}"
      os.system("date")
      print(cmd)
      os.system(f"{cmd} > {log_file}")
