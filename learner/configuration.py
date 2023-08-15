import argparse
import representation

""" Parameters for the GNN. Note some features are deprecated. """


def create_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument('--device', type=int, default=0)
    parser.add_argument('-d', '--domain', default="goose-pretraining")
    parser.add_argument('-t', '--task', default='h', choices=["h", "a"], help="predict value or action")

    # model params
    parser.add_argument('-m', '--model', type=str)
    parser.add_argument('-L', '--nlayers', type=int, default=16)
    parser.add_argument('-H', '--nhid', type=int, default=64)
    parser.add_argument('--share-layers', action='store_true')
    parser.add_argument('--aggr', type=str, default="mean")
    parser.add_argument('--pool', type=str, default="sum")
    parser.add_argument('--drop', type=float, default=0.0, help="probability of an element to be zeroed")
    parser.add_argument('--vn', help='virtual node', action='store_true')

    # optimisation params
    parser.add_argument('--loss', type=str, choices=["mse", "wmse", "pemse"], default="mse")
    parser.add_argument('--lr', type=float, default=0.001)
    parser.add_argument('--patience', type=int, default=10)
    parser.add_argument('--reduction', type=float, default=0.1)
    parser.add_argument('--batch-size', type=int, default=16)
    parser.add_argument('--epochs', type=int, default=2000)

    # data arguments
    parser.add_argument('-r', '--rep', type=str, choices=representation.REPRESENTATIONS)
    parser.add_argument('-n', '--max-nodes', type=int, default=-1, 
                        help="max nodes for generating graphs (-1 means no bound)")
    parser.add_argument('-c', '--cutoff', type=int, default=-1, 
                        help="max cost to learn (-1 means no bound)")
    parser.add_argument('-s', '--strategy', choices=["init", "random", "entire"], default="entire", 
                        help='sample strategies')
    parser.add_argument('--small-train', action="store_true", 
                        help="Small train set: useful for debugging.")

    # data feature augmentations (deprecated)
    parser.add_argument('-f', '--features', type=str, default='none', choices=representation.node_features.NODE_FEAT.keys())
    parser.add_argument('--rni-size', type=float, choices=representation.node_features.RNI_SIZE)
    parser.add_argument('--rni-dist', type=str, choices=representation.node_features.RNI_DIST)
    parser.add_argument('--lpe-k', type=int)

    # save file
    parser.add_argument('--save-file', dest="save_file", type=str, default=None)

    # anti verbose
    parser.add_argument('--no-tqdm', dest='tqdm', action='store_false')
    parser.add_argument('--fast-train', action='store_true', help="ignore some additional computation of stats")

    return parser

def check_config(args):
  pass  # TODO check model is compatible with representation
  