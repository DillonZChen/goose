import sys

sys.path.append("..")
import os
import random
import numpy as np
from tqdm import tqdm
from torch_geometric.data import Data
from torch_geometric.loader import DataLoader
from sklearn.model_selection import train_test_split
from util.stats import get_stats
from representation import REPRESENTATIONS

# TODO(DZC) version for FD and PWL (the latter does remove statics from states)
_DOWNWARD = "./../downward/fast-downward.py"
_POWERLIFTED = "./../powerlifted/powerlifted.py"


def get_plan_info(domain_pddl, problem_pddl, plan_file, planner):
    states = []
    actions = []

    with open(plan_file, "r") as f:
        for line in f.readlines():
            if ";" in line:
                continue
            actions.append(line.replace("\n", ""))

    state_output_file = f"{domain_pddl}+{problem_pddl}+{plan_file}+{planner}"
    state_output_file = state_output_file.replace("/", "-").replace(".", "-")
    state_output_file = state_output_file + ".states"

    cmd = {
        "pwl": f"export PLAN_PATH={plan_file} "
        + f"&& {_POWERLIFTED} -d {domain_pddl} -i {problem_pddl} -s perfect "
        + f"--plan-file {state_output_file}",
        "fd": f"export PLAN_INPUT_PATH={plan_file} "
        + f"&& export STATES_OUTPUT_PATH={state_output_file} "
        + f"&& {_DOWNWARD} {domain_pddl} {problem_pddl} "
        + f'--search \'perfect([linear_regression(model_data="", graph_data="")])\'',  # need filler h
    }[planner]
    output = os.popen(cmd).readlines()
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
    os.remove(state_output_file)

    ret = []
    for i, state in enumerate(states):
        if i == len(actions):
            continue  # ignore the goal state, annoying for learning useful schema
        distance_to_goal = len(states) - i - 1
        action = actions[i]
        ret.append((state, action, distance_to_goal))
    return ret


def get_graphs_from_plans(representation, planner, domain_pddl, tasks_dir, plans_dir):
    print("Generating graphs from plans...")
    graphs = []

    for plan_file in tqdm(list(os.listdir(plans_dir))):
        problem_pddl = f"{tasks_dir}/{plan_file.replace('.plan', '.pddl')}"
        assert os.path.exists(problem_pddl), problem_pddl
        plan_file = f"{plans_dir}/{plan_file}"
        rep = REPRESENTATIONS[representation](domain_pddl, problem_pddl)
        
        # rep.convert_to_pyg()
        rep.convert_to_coloured_graph()
        plan = get_plan_info(domain_pddl, problem_pddl, plan_file, planner)

        for s, action, distance_to_goal in plan:
            if REPRESENTATIONS[representation].lifted:
                s = rep.str_to_state(s)

            graph = rep.state_to_cgraph(s)
            graphs.append((graph, distance_to_goal))

    print("Graphs generated!")
    return graphs


def get_dataset_from_args(args):
    rep = args.rep
    planner = args.planner
    small_train = args.small_train

    domain_pddl = args.domain_pddl
    tasks_dir = args.tasks_dir
    plans_dir = args.plans_dir

    dataset = get_graphs_from_plans(rep, planner, domain_pddl, tasks_dir, plans_dir)
    if small_train:
        random.seed(123)
        dataset = random.sample(dataset, k=1000)

    get_stats(dataset=dataset, desc="Whole dataset")

    graphs = [data[0] for data in dataset]
    y = np.array([data[1] for data in dataset])

    return graphs, y
