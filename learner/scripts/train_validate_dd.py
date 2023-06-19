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


def pwl_cmd(domain_name, df, pf, m, search, seed, timeout=600):
  os.makedirs("lifted", exist_ok=True)
  os.makedirs("plan", exist_ok=True)
  description = f"{domain_name}_{os.path.basename(pf).replace('.pddl','')}_{search}_{os.path.basename(m).replace('.dt', '')}"
  output_file = f"lifted/{description}.lifted"
  plan_file = f"plans/{description}.plan"
  cmd = f"./../powerlifted.py " \
        f"-d {df} " \
        f"-i {pf} " \
        f"-m {m} " \
        f"-e gnn " \
        f"-s {search} " \
        f"--time-limit {timeout} " \
        f"--seed {seed} " \
        f"--translator-output-file {output_file}" \
        f"--plan {plan_file}"
  return cmd


def main():
  parser=argparse.ArgumentParser()
  parser.add_argument("domain", type=str, choices=GOOSE_DOMAINS)
  args = parser.parse_args()
  domain = args.domain
  for repeat in range(REPEATS):
    for val_repeat in range(VAL_REPEATS):
      for rep in EXPERIMENT_REPRESENTATIONS:
        model = "RGNN" if CONFIG[rep]['edge_labels'] else "MPNN"
        model_file = f"dd_{rep}_{domain}_L{L}_H{H}_p{patience}_v{val_repeat}_r{repeat}"
        
        # train
        if not os.path.exists(f"trained_models/{model_file}.dt"):
          log_dir = f"logs/train"
          os.makedirs(log_dir, exist_ok=True)
          log_file = f"{log_dir}/{model_file}.log"
          cmd = f"python3 train.py --fast-train --no-tqdm -r {rep} -m {model} -d goose-{domain}-only -L {L} -H {H} --patience {patience} --save-file {model_file}"
          os.system("date")
          print("training")
          print(cmd)
          os.system(f"{cmd} > {log_file}")

        # validate
        log_dir = f"logs/val"
        os.makedirs(log_dir, exist_ok=True)
        df = f"../benchmarks/goose/{domain}/domain.pddl"
        val_dir = f"../benchmarks/goose/{domain}/val"
        for pf in os.listdir(val_dir):
          log_file = f"{log_dir}/{pf.replace('.pddl', '')}_{model_file}.log"
          if not os.path.exists(log_file):
            cmd = pwl_cmd(domain, df, pf, model_file, "gbbfs", 0)
            os.system("date")
            print("validating")
            print(cmd)
            os.system(f"{cmd} > {log_file}")


if __name__ == "__main__":
  main()
