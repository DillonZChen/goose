import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from representation import CONFIG, REPRESENTATIONS
from dataset.graphs import gen_graph_rep
import argparse

VAL_REPEATS = 3
DOMAIN = "gripper"

L=8
H=64
patience=10


def pwl_cmd(domain_name, df, pf, m, search, seed, timeout=120):
  os.makedirs("lifted", exist_ok=True)
  os.makedirs("plan", exist_ok=True)
  description = f"{domain_name}_{os.path.basename(pf).replace('.pddl','')}_{search}_{os.path.basename(m).replace('.dt', '')}"
  lifted_file = f"lifted/{description}.lifted"
  plan_file = f"plans/{description}.plan"
  cmd = f"./../powerlifted/powerlifted.py " \
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
  parser.add_argument("--graph-only", action='store_true', dest="graph_only")
  args = parser.parse_args()
  rep = args.rep

  gen_graph_rep(representation=rep, regenerate=True, domain=f"goose-{DOMAIN}")

  if args.graph_only:
    return

  os.system("rm -rf logs/tests")

  for val_repeat in range(VAL_REPEATS):
    model = "RGNN" if CONFIG[rep]['edge_labels'] else "MPNN"
    model_file = f"test_{DOMAIN}_{rep}_{val_repeat}"
    
    # train
    cmd = f"python3 train.py --fast-train -r {rep} -m {model} -d goose-{DOMAIN}-only -L {L} -H {H} --patience {patience} --save-file {model_file}"
    os.system("date")
    print("training")
    print(cmd)
    os.system(cmd)

    # validate
    log_dir = f"logs/tests"
    os.makedirs(log_dir, exist_ok=True)
    df = f"../benchmarks/goose/{DOMAIN}/domain.pddl"
    val_dir = f"../benchmarks/goose/{DOMAIN}/val"
    for f in os.listdir(val_dir):
      pf = f"{val_dir}/{f}"
      cmd,lifted_file = pwl_cmd(DOMAIN, df, pf, f"trained_models/{model_file}", "gbbfs", 0)
      os.system("date")
      print("validating")
      print(cmd)
      os.system(cmd)
      os.remove(lifted_file)


if __name__ == "__main__":
  main()
