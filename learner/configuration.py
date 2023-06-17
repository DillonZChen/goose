import argparse
import representation


def create_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument('--device', type=int, default=0)
    parser.add_argument('-d', '--domain', default="goose-pretraining")
    parser.add_argument('--tar-domain', dest="tar_domain", help="target domain for domain adaptation", default=None)
    parser.add_argument('-t', '--task', default='h', choices=["h", "a"], help="predict value or action")

    # model params
    parser.add_argument('-m', '--model', type=str)
    parser.add_argument('-L', '--nlayers', type=int, default=16)
    parser.add_argument('-H', '--nhid', type=int, default=64)

    parser.add_argument('--nheads', type=int, default=2)
    parser.add_argument('--share-layers', action='store_true')
    parser.add_argument('--pool', type=str, default="mean")
    parser.add_argument('--drop', type=float, default=0.0, help="probability of an element to be zeroed")
    parser.add_argument('--double', action='store_true')
    parser.add_argument('--vn', help='virtual node', action='store_true')

    # optimisation params
    parser.add_argument('--loss', type=str, choices=["mse", "wmse", "pemse", "bce", "mmd", "coral"], default="mse")
    parser.add_argument('-l', '--loss-weight', type=float, default=10, help='loss weight for loss functions such as CORAL')
    parser.add_argument('--decay', type=float, default=0)
    parser.add_argument('--lr', type=float, default=0.001)
    parser.add_argument('--patience', type=int, default=10)
    parser.add_argument('--reduction', type=float, default=0.1)
    parser.add_argument('--batch-size', type=int, default=16)
    parser.add_argument('--epochs', type=int, default=2000)

    # data arguments
    parser.add_argument('-u', '--heuristic', type=str, default="opt", help="heuristic to learn")
    parser.add_argument('-r', '--rep', type=str, choices=representation.REPRESENTATIONS)
    parser.add_argument('-n', '--max-nodes', type=int, default=-1, help="max nodes for generating graphs (-1 means no bound)")
    parser.add_argument('-c', '--cutoff', type=int, default=-1, help="max cost to learn (-1 means no bound)")
    parser.add_argument('--no-val', dest="val", action="store_false", help="no validation set")
    parser.add_argument('--test', dest="test", action="store_true", help="have test set")

    parser.add_argument('-f', '--features', type=str, default='none', choices=representation.node_features.NODE_FEAT.keys())
    parser.add_argument('--rni-size', type=float, choices=representation.node_features.RNI_SIZE)
    parser.add_argument('--rni-dist', type=str, choices=representation.node_features.RNI_DIST)
    parser.add_argument('--lpe-k', type=int)

    parser.add_argument('-s', '--strategy', choices=["init", "random", "entire"], default="entire", help='sample strategies')
    parser.add_argument('--small-train', action="store_true")

    parser.add_argument('--save-file', dest="save_file", type=str, default=None)
    parser.add_argument('--pretrained', type=str)

    parser.add_argument('--no-tqdm', dest='tqdm', action='store_false')
    parser.add_argument('--fast-train', action='store_true', help="ignore some additional computation of stats")

    return parser

def check_config(args):
  if args.pretrained is None:
    assert args.model is not None
    assert args.rep is not None
    args.directed = representation.CONFIG[args.rep]["directed"]
    args.edge_labels = representation.CONFIG[args.rep]["edge_labels"]
    # if args.edge_labels:
    #   assert "EL" in args.model
    # else:
    #   assert "EL" not in args.model
  else:
    assert args.model is None  # model and rep defined in pretrained args
    assert args.rep is None
  if not args.val:
    raise ValueError("--no-val is deprecated")
  