import os
import argparse
from itertools import product

"""
2 SU per CPU/4GB per hour
~> 30 minute + 8GB job = 2 SU
"""

# 900 all problems / 300 per difficulty
# 1.8 KSU all problems / 0.6 KSU per difficulty
_DOMAINS = [
  "blocksworld",
  "childsnack",
  "ferry",
  "floortile",
  "miconic",
  "rovers",
  "satellite",
  "sokoban",
  "spanner",
  "transport",
]
_DIFFICULTIES = [
  "easy",
  "medium",
  "hard",
]

_REPRESENTATIONS = ["llg"]
_ITERATIONS = [0]

_TIMEOUT=1800

_LOG_DIR = "logs_gnn/test_ipc2023"
_LOCK_DIR = "lock"
_AUX_DIR = "/scratch/sv11/dc6693/aux"
_MODEL_DIR = "/scratch/sv11/dc6693/trained_models_gnn"

os.makedirs(_LOG_DIR, exist_ok=True)
os.makedirs(_LOCK_DIR, exist_ok=True)
os.makedirs(_AUX_DIR, exist_ok=True)
    
def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('-e', type=int, default=1)
  args = parser.parse_args()
  to_go = 0
  skipped = 0
  submitted = 0

  e = args.e

  def submit(domain, df, pf, difficulty, model_file):
    nonlocal to_go
    nonlocal skipped
    nonlocal submitted
    nonlocal e
    
    problem = os.path.basename(pf).replace(".pddl", "")

    # check whether to skip
    desc = f'{domain}_{difficulty}_{problem}_{model_file.replace("/", "-")}'
    log_file = f'{_LOG_DIR}/{desc}.log'
    lock_file = f'{_LOCK_DIR}/{desc}.lock'
    plan_file = f'{_AUX_DIR}/{desc}.plan'
    aux_file = f"{_AUX_DIR}/{desc}.aux"

    if os.path.exists(log_file) or os.path.exists(plan_file) or os.path.exists(lock_file):
      skipped += 1
      return

    if submitted >= e:
      to_go += 1
      return

    # submit
    with open(lock_file, 'w') as f:
      pass

    cmd = f'qsub -o {log_file} -j oe -v '+\
          f'DOM_PATH="{df}",'+\
          f'INS_PATH="{pf}",'+\
          f'MODEL_PATH="{model_file}",'+\
          f'TIMEOUT="{_TIMEOUT}",'+\
          f'AUX_FILE="{aux_file}",'+\
          f'PLAN_FILE="{plan_file}",'+\
          f'LOCK_FILE="{lock_file}" '+\
          f'scripts_pbs/gnn_job.sh'
    os.system(cmd)
    submitted += 1
    return

  CONFIGS = list(product(
    _REPRESENTATIONS, 
    _ITERATIONS,
    _DOMAINS,
    _DIFFICULTIES,
  ))

  for config in CONFIGS:
    rep, iteration, domain, difficulty = config
    model_file = f"{_MODEL_DIR}/ipc2023-learning-{domain}-{iteration}"
    df = f"../benchmarks/ipc2023-learning-benchmarks/{domain}/domain.pddl"
    problem_dir = f"../benchmarks/ipc2023-learning-benchmarks/{domain}/testing/{difficulty}"
    for file in sorted(os.listdir(problem_dir)):
      pf = f"{problem_dir}/{file}"
      submit(domain, df, pf, difficulty, model_file)
  print("submitted:", submitted)    
  print("skipped:", skipped)    
  print("to_go:", to_go)

if __name__ == "__main__":
  main()
