import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
import argparse


if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument('rep', type=str, help="graph representation to generate")
  args = parser.parse_args()
  rep = args.rep

  print(f"Are you sure you want to delete all stored data, logs and models associated with {rep}? [N/y]")
  user_input = input()

  if user_input=='y':
    print(f"Deleting all files associated with {rep}...")
    os.system(f"rm -rf data/graphs/{rep}")
    os.system(f"rm -rf logs/*/*_{rep}_*")
    os.system(f"rm -rf logs/*_{rep}.log")
    os.system(f"rm -rf trained_models/*_{rep}_*")
    os.system(f"rm -rf validated_models/*_{rep}_*")
    print(f"Deleted all files associated with {rep}!")