import random
import os
from itertools import product
from tqdm import tqdm
from torch_geometric.data import Data
from torch_geometric.loader import DataLoader
from sklearn.model_selection import train_test_split
from representation import REPRESENTATIONS
from util.stats import get_stats

_DOWNWARD = "./../planners/downward/fast-downward.py"
_POWERLIFTED = "./../planners/powerlifted/powerlifted.py"
_BENCHMARKS_DIR = "../dataset"  # assume script called from learner directory

_MAX_Y = 64


def get_plan_info(domain_pddl, problem_pddl, plan_file, args):
    planner = args.planner

    states = []
    actions = []

    with open(plan_file, "r") as f:
        for line in f.readlines():
            if ";" in line:
                continue
            actions.append(line.replace("\n", ""))

    aux_garbage = repr(hash((domain_pddl, problem_pddl, plan_file, repr(args))))
    aux_garbage = aux_garbage.replace("-", "n")
    state_output_file = aux_garbage + ".states"
    sas_file = aux_garbage + ".sas"

    cmd = {
        "pwl": f"export PLAN_PATH={plan_file} "
        + f"&& {_POWERLIFTED} -d {domain_pddl} -i {problem_pddl} -s perfect "
        + f"--plan-file {state_output_file}",
        "fd": f"export PLAN_INPUT_PATH={plan_file} "
        + f"&& export STATES_OUTPUT_PATH={state_output_file} "
        + f"&& {_DOWNWARD} --sas-file {sas_file} {domain_pddl} {problem_pddl} "
        + f"--search 'perfect([blind()])'",  # need filler h
    }[planner]
    output = os.popen(cmd).readlines()
    if output:
        pass  # this is so syntax highlighting sees `output`
    # os.system(cmd)
    with open(state_output_file, "r") as f:
        for line in f.readlines():
            if ";" in line:
                continue
            line = line.replace("\n", "")
            s = set()
            for fact in line.split():
                if "(" not in fact:
                    lime = f"({fact})"
                else:
                    pred = fact[: fact.index("(")]
                    fact = fact.replace(pred + "(", "").replace(")", "")
                    args = fact.split(",")[:-1]
                    lime = "(" + " ".join([pred] + args) + ")"
                s.add(lime)
            states.append(s)
    if os.path.exists(sas_file):
        os.remove(sas_file)
    if os.path.exists(state_output_file):
        os.remove(state_output_file)

    schema_cnt = {}
    for action in actions:
        schema = action.replace("(", "").split()[0]
        if schema not in schema_cnt:
            schema_cnt[schema] = 0
        schema_cnt[schema] += 1

    ret = []
    for i, state in enumerate(states):
        if i == len(actions):
            ret.append((state, {0: 0}))
        else:
            action = actions[i]
            schema = action.replace("(", "").split()[0]
            ret.append((state, schema_cnt.copy()))
            schema_cnt[schema] -= 1
    return ret


def get_graphs_from_plan(domain_pddl, problem_pddl, plan_file, args):
    graphs = []
    rep = REPRESENTATIONS[args.rep](domain_pddl=domain_pddl, problem_pddl=problem_pddl)
    rep.convert_to_pyg()
    plan = get_plan_info(domain_pddl, problem_pddl, plan_file, args)

    for state, schema_cnt in plan:
        state = rep.str_to_state(state)
        x, edge_index = rep.state_to_tensor(state)
        y = sum(schema_cnt.values())
        if y > _MAX_Y:
            continue
        graph = Data(x=x, edge_index=edge_index, y=y)
        graphs.append(graph)
    return graphs


def get_ipc_graphs(args):
    graphs = []

    ipcs = list(set(os.listdir(f"{_BENCHMARKS_DIR}/ipc")).difference({"README.md"}))
    ipc_domains = []
    for ipc in ipcs:
        for domain in os.listdir(f"{_BENCHMARKS_DIR}/ipc/{ipc}/domains"):
            ipc_domains.append((ipc, domain))

    pbar = tqdm(sorted(ipc_domains))
    for ipc, domain in pbar:
        pbar.set_description(f"{ipc}-{domain}")
        domain_pddl = f"{_BENCHMARKS_DIR}/ipc/{ipc}/domains/{domain}/domain.pddl"
        tasks_dir = f"{_BENCHMARKS_DIR}/ipc/{ipc}/domains/{domain}/instances"
        plans_dir = f"{_BENCHMARKS_DIR}/ipc/{ipc}/domains/{domain}/solutions"
        try:
            assert os.path.exists(domain_pddl)
            assert os.path.exists(tasks_dir)
            assert os.path.exists(plans_dir)
            graphs += get_graphs_from_dir(domain_pddl, tasks_dir, plans_dir, args)
        except AssertionError as e:  # cannot parse some domains
            print(f"skipped graphs for {domain}")
            pass

    return graphs


def get_graphs_from_dir(domain_pddl, tasks_dir, plans_dir, args):
    graphs = []
    for plan_file in sorted(list(os.listdir(plans_dir))):
        problem_pddl = f"{tasks_dir}/{plan_file.replace('.plan', '.pddl')}"
        assert os.path.exists(problem_pddl), problem_pddl
        plan_file = f"{plans_dir}/{plan_file}"
        graphs += get_graphs_from_plan(domain_pddl, problem_pddl, plan_file, args)
    return graphs


def get_graphs_from_args(args):
    print("Generating graphs from plans...")

    domain = args.domain

    if domain == "ipc":
        graphs = get_ipc_graphs(args)
    else:
        domain_pddl = f"{_BENCHMARKS_DIR}/goose/{domain}/domain.pddl"
        tasks_dir = f"{_BENCHMARKS_DIR}/goose/{domain}/train"
        plans_dir = f"{_BENCHMARKS_DIR}/goose/{domain}/train_solution"
        graphs = get_graphs_from_dir(domain_pddl, tasks_dir, plans_dir, args)

    print("Graphs generated!")
    return graphs


def get_loaders_from_args_gnn(args):
    batch_size = args.batch_size
    small_train = args.small_train

    dataset = get_graphs_from_args(args)
    if small_train:
        random.seed(123)
        dataset = random.sample(dataset, k=1000)

    trainset, valset = train_test_split(dataset, test_size=0.15, random_state=4550)

    get_stats(dataset=dataset, desc="Whole dataset")
    get_stats(dataset=trainset, desc="Train set")
    get_stats(dataset=valset, desc="Val set")
    print("train size:", len(trainset))
    print("validation size:", len(valset))

    train_loader = DataLoader(
        trainset,
        batch_size=batch_size,
        shuffle=True,
    )
    val_loader = DataLoader(
        valset,
        batch_size=batch_size,
        shuffle=False,
    )

    return train_loader, val_loader
