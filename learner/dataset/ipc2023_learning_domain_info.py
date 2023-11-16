import os
import re
from tqdm import tqdm


IPC2023_LEARNING_DOMAINS = [  # 90 test instances each
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


def sorted_nicely(l):
    """Sort the given iterable in the way that humans expect."""
    convert = lambda text: int(text) if text.isdigit() else text
    alphanum_key = lambda key: [convert(c) for c in re.split("([0-9]+)", key)]
    return sorted(l, key=alphanum_key)


def get_train_ipc2023_learning_instance_files(domain=None):
    ret = []
    if domain is None:
        domains = sorted_nicely(IPC2023_LEARNING_DOMAINS)
    else:
        domains = [domain]
    for domain in domains:
        domain_dir = f"../benchmarks/ipc2023-learning-benchmarks/{domain}"
        df = f"{domain_dir}/domain.pddl"
        for file in sorted_nicely(os.listdir(f"{domain_dir}/training/easy")):
            pf = f"{domain_dir}/training/easy/{file}"
            ret.append((f"ipc2023-learning-{domain}", df, pf))
    # print(f"num goose train instances: {len(ret)}")
    return ret


def get_plan_file_from_train_instance(domain, pf):
    return f"../benchmarks/ipc2023-learning-benchmarks/solutions/{domain.replace('ipc2023-learning-', '')}/training/easy/{os.path.basename(pf).replace('.pddl', '')}.plan"


def plans_to_states():
    # converts training plans into states
    for domain, df, pf in tqdm(get_train_ipc2023_learning_instance_files()):
        plan = get_plan_file_from_train_instance(domain, pf)

        state_dir = f"data/plan_objects/{domain}"
        state_dir2 = f"data/plan_objects_check/{domain}"
        os.makedirs(state_dir, exist_ok=True)
        os.makedirs(state_dir2, exist_ok=True)
        state_file = f"{state_dir}/{os.path.basename(pf).replace('.pddl', '')}.states"
        state_file2 = f"{state_dir2}/{os.path.basename(pf).replace('.pddl', '')}.states"

        # cmd = f"export PLAN_PATH={plan} && singularity exec ../goose.sif ./../powerlifted/powerlifted.py --gpu -d {df} -i {pf} -s perfect --plan-file {state_file2}"
        # log = os.popen(cmd).readlines()

        cmd = (
            f"export PLAN_INPUT_PATH={plan} && export STATES_OUTPUT_PATH={state_file} && "
            + f'./../downward/fast-downward.py {df} {pf} --search \'perfect([kernel(model_data="a", graph_data="a")])\''
        )
        log = os.popen(cmd).readlines()

        # print(domain)
        # os.system(f"{cmd} > {domain}-{os.path.basename(state_file).replace('.states', '')}.log")
        # os.system(cmd)
        # breakpoint()
    return


def get_number_of_ipc2023_training_data():
    ret = {}
    for domain in sorted_nicely(IPC2023_LEARNING_DOMAINS):
        states = 0
        for _, _, pf in get_train_ipc2023_learning_instance_files(domain):
            states += get_plan_length(get_plan_file_from_train_instance(domain, pf))
        ret[domain] = states
    return ret


def get_test_ipc2023_learning_instance_files():
    ret = {}
    for domain in sorted_nicely(IPC2023_LEARNING_DOMAINS):
        ret[domain] = []
        domain_dir = f"../benchmarks/ipc2023-learning-benchmarks/{domain}"
        df = f"{domain_dir}/domain.pddl"
        for difficulty in ["easy", "medium", "hard"]:
            for file in sorted_nicely(os.listdir(f"{domain_dir}/testing/{difficulty}")):
                pf = f"{domain_dir}/testing/{difficulty}/{file}"
                ret[domain].append((df, pf))
    return ret


def get_plan_length(plan_file):
    lines = open(plan_file, "r").readlines()
    plan_cost = len(lines) - 1
    assert ";" in lines[-1], f"{lines[-1]} {plan_file}"
    return plan_cost


def get_best_bound(domain, difficulty, problem_name):
    f = f"../benchmarks/ipc2023-learning-benchmarks/solutions/{domain}/testing/{difficulty}/{problem_name}.plan"
    return get_plan_length(f)


if __name__ == "__main__":
    ret = get_test_ipc2023_learning_instance_files()
    for domain in sorted_nicely(IPC2023_LEARNING_DOMAINS):
        print(domain, len(ret[domain]))
    pass
