import os
from tqdm import tqdm


IPC2023_LEARNING_DOMAINS = [
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

def get_train_ipc2023_learning_instance_files():
    ret = []
    for domain in sorted(IPC2023_LEARNING_DOMAINS):
      domain_dir = f"../benchmarks/ipc2023-learning-benchmarks/{domain}"
      df = f"{domain_dir}/domain.pddl"
      for file in sorted(os.listdir(f'{domain_dir}/training/easy')):
        pf = f"{domain_dir}/training/easy/{file}"
        ret.append((f'ipc2023-learning-{domain}', df, pf))
    # print(f"num goose train instances: {len(ret)}")
    return ret

if __name__ == "__main__":
  for domain, df, pf in tqdm(get_train_ipc2023_learning_instance_files()):

    plan = f"../benchmarks/ipc2023-learning-benchmarks/solutions/{domain.replace('ipc2023-learning-', '')}/training/easy/{os.path.basename(pf).replace('.pddl', '')}.plan"
    
    state_dir = f"data/plan_objects/{domain}"
    state_dir2 = f"data/plan_objects_check/{domain}"
    os.makedirs(state_dir, exist_ok=True)
    os.makedirs(state_dir2, exist_ok=True)
    state_file = f"{state_dir}/{os.path.basename(pf).replace('.pddl', '')}.states"
    state_file2 = f"{state_dir2}/{os.path.basename(pf).replace('.pddl', '')}.states"

    # cmd = f"export PLAN_PATH={plan} && singularity exec ../goose.sif ./../powerlifted/powerlifted.py --gpu -d {df} -i {pf} -s perfect --plan-file {state_file2}"
    # log = os.popen(cmd).readlines()
    
    cmd = f"export PLAN_INPUT_PATH={plan} && export STATES_OUTPUT_PATH={state_file} && " + \
          f"./../downward/fast-downward.py {df} {pf} --search 'perfect([kernel(model_data=\"a\", graph_data=\"a\")])'"
    log = os.popen(cmd).readlines()
    
    # print(domain)
    # os.system(f"{cmd} > {domain}-{os.path.basename(state_file).replace('.states', '')}.log")
    # os.system(cmd)
    # breakpoint()
