import os
import argparse


def main():
  parser = argparse.ArgumentParser()

  parser.add_argument('-m', type=str, help='model params file name in trained_models', required=True)

  args = parser.parse_args()

  for n_lo, n_hi, c_lo, c_hi in [
    (-1,    10000, 32, -1),
    (10000, 50000, -1, 32),
    (10000, 50000, 32, -1),
  ]:
    cmd = f"python3 visualise/model_prediction.py -m {args.m} --n_lo {n_lo} --n_hi {n_hi} --c_lo {c_lo} --c_hi {c_hi}"
    os.system(cmd)

if __name__ == "__main__":
  main()
