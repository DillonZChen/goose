import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
import argparse
from representation import REPRESENTATIONS
from dataset.graphs_gnn import gen_graph_rep


if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument('rep', type=str, help="graph representation to generate", choices=REPRESENTATIONS)
  parser.add_argument('-d', '--domain', type=str, help="domain to generate (useful for debugging)")
  parser.add_argument('--regenerate', action="store_true")
  args = parser.parse_args()

  rep = args.rep
  gen_graph_rep(representation=rep,
                regenerate=args.regenerate,
                domain=args.domain)
    