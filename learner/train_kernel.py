""" Main training pipeline script. """

import time
import argparse
import representation
import kernels
from dataset.dataset import get_dataset_from_args_kernels
from util.save_load import print_arguments


def create_parser():
  parser = argparse.ArgumentParser()

  parser.add_argument('-r', '--rep', type=str, required=True, choices=representation.REPRESENTATIONS,
                      help="graph representation to use")
  parser.add_argument('-k', '--kernel', type=str, required=True, choices=kernels.KERNELS,
                      help="graph representation to use")
  parser.add_argument('-l', '--iterations', type=int, default=5,
                      help="number of iterations for kernel algorithms")
  parser.add_argument('-d', '--domain', type=str, default="goose-di",
                      help="domain to train on; defaults to goose-di which is di training")
  parser.add_argument('--small-train', action="store_true", 
                      help="use small train set, useful for debugging")
  parser.add_argument('--save-file', dest="save_file", type=str, default=None,
                      help="file to save model weights")
  
  return parser

if __name__ == "__main__":
  parser = create_parser()
  args = parser.parse_args()
  print_arguments(args)

  dataset = get_dataset_from_args_kernels(args)
  
  kernel = kernels.KERNELS[args.kernel](args.iterations)
  kernel.read_data([data[0] for data in dataset])

  