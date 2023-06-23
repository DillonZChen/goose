import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from representation import CONFIG, REPRESENTATIONS
from dataset.graphs import gen_graph_rep
from dataset.goose_domain_info import GOOSE_DOMAINS
from util.search import search_cmd
import argparse

VAL_REPEATS = 3

L=8
H=64
patience=10
aggr="mean"


def main():
  parser=argparse.ArgumentParser()
  parser.add_argument("domain", type=str, choices=GOOSE_DOMAINS)
  parser.add_argument("rep", type=str, choices=REPRESENTATIONS)
  parser.add_argument("--graph-only", action='store_true', dest="graph_only", help="Generating graphs only. No training and search.")
  args = parser.parse_args()
  domain = args.domain
  rep = args.rep

  if args.graph_only:
    gen_graph_rep(representation=rep, regenerate=True, domain=f"goose-{domain}")
    return

  for val_repeat in range(VAL_REPEATS):
    model = "RGNN" if CONFIG[rep]['edge_labels'] else "MPNN"
    model_file = f"test_{domain}_{rep}_{val_repeat}"
    
    # train
    cmd = f"python3 train.py --fast-train -r {rep} -m {model} -d goose-{domain}-only -L {L} -H {H} --aggr {aggr} --patience {patience} --save-file {model_file}"
    os.system("date")
    print("training")
    print(cmd)
    os.system(cmd)

    # validate
    df = f"../benchmarks/goose/{domain}/domain.pddl"
    val_dir = f"../benchmarks/goose/{domain}/val"
    for f in os.listdir(val_dir):
      pf = f"{val_dir}/{f}"
      cmd,lifted_file = search_cmd(rep, domain, df, pf, f"trained_models/{model_file}", "gbbfs", 0, timeout=60)
      os.system("date")
      print("validating")
      print(cmd)
      os.system(cmd)
      os.remove(lifted_file)


if __name__ == "__main__":
  main()
