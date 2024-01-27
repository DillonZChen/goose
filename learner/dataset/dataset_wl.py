import sys

sys.path.append("..")
import os
import random
import numpy as np
from tqdm import tqdm
from util.stats import get_stats
from representation import REPRESENTATIONS

# from deadend.deadend import deadend_states

_DOWNWARD = "./../planners/downward_cpu/fast-downward.py"
_POWERLIFTED = "./../planners/powerlifted/powerlifted.py"

ALL_KEY = "_all_"


def sample_from_dict(d, sample, seed):
    random.seed(seed)
    keys = random.sample(list(d), sample)
    values = [d[k] for k in keys]
    return dict(zip(keys, values))


def get_plan_info(domain_pddl, problem_pddl, plan_file, args):
    states = []
    actions = []

    planner = args.planner

    with open(plan_file, "r") as f:
        for line in f.readlines():
            if ";" in line:
                continue
            actions.append(line.replace("\n", ""))

    state_output_file = repr(hash(repr(args))).replace("-", "n")
    state_output_file += (
        repr(hash(domain_pddl))
        + repr(hash(problem_pddl))
        + repr(hash(plan_file))
    )
    aux_file = state_output_file + ".sas"
    state_output_file = state_output_file + ".states"

    cmd = {
        "pwl": f"export PLAN_PATH={plan_file} "
        + f"&& {_POWERLIFTED} -d {domain_pddl} -i {problem_pddl} -s perfect "
        + f"--plan-file {state_output_file}",
        "fd": f"export PLAN_INPUT_PATH={plan_file} "
        + f"&& export STATES_OUTPUT_PATH={state_output_file} "
        + f"&& {_DOWNWARD} --sas-file {aux_file} {domain_pddl} {problem_pddl} "
        + f"--search 'perfect([blind()])'",  # need filler h
    }[planner]

    # print("generating plan states with:") print(cmd)

    # disgusting method which hopefully makes running in parallel work fine
    assert not os.path.exists(aux_file), aux_file
    assert not os.path.exists(state_output_file), state_output_file
    output = os.popen(cmd).readlines()
    if output == None:
        print("make this variable seen")
    if os.path.exists(aux_file):
        os.remove(aux_file)

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
            states.append(sorted(list(s)))
    os.remove(state_output_file)

    schema_cnt = {ALL_KEY: len(actions)}
    for action in actions:
        schema = action.replace("(", "").split()[0]
        if schema not in schema_cnt:
            schema_cnt[schema] = 0
        schema_cnt[schema] += 1

    ret = []
    for i, state in enumerate(states):
        if i == len(actions):
            continue  # ignore the goal state, annoying for learning useful schema
        action = actions[i]
        schema = action.replace("(", "").split()[0]
        ret.append((state, schema_cnt.copy()))
        # print(state)
        # for s, v in schema_cnt.items():
        #     print(f"{s:>15} {v:>4}")
        # print()
        schema_cnt[schema] -= 1
        schema_cnt[ALL_KEY] -= 1
    # breakpoint()
    return ret


def get_graphs_from_plans(args):
    print("Generating graphs from plans...")
    dataset = []  # can probably make a class for this

    schema_keys = set()

    representation = args.rep
    domain_pddl = args.domain_pddl
    tasks_dir = args.tasks_dir
    plans_dir = args.plans_dir

    for plan_file in tqdm(sorted(list(os.listdir(plans_dir)))):
        problem_pddl = f"{tasks_dir}/{plan_file.replace('.plan', '.pddl')}"
        assert os.path.exists(problem_pddl), problem_pddl
        plan_file = f"{plans_dir}/{plan_file}"
        rep = REPRESENTATIONS[representation](domain_pddl, problem_pddl)

        plan = get_plan_info(domain_pddl, problem_pddl, plan_file, args)

        for s, schema_cnt in plan:
            s = rep.str_to_state(s)
            graph = rep.state_to_cgraph(s)
            dataset.append((graph, schema_cnt))
            schema_keys = schema_keys.union(set(schema_cnt.keys()))

    print("Graphs generated!")
    return dataset, schema_keys


def get_dataset_from_args(args):
    """Returns list of graphs, and dictionaries where keys are given by h* and schema counts"""
    dataset, schema_keys = get_graphs_from_plans(args)

    graphs = []
    ys = []

    for graph, schema_cnt in dataset:
        graphs.append(graph)
        test = 0
        for k in schema_keys:
            if k not in schema_cnt:
                schema_cnt[k] = 0  # probably should never happen?
            test += schema_cnt[k] if k != ALL_KEY else 0
        assert test == schema_cnt[ALL_KEY]
        ys.append(schema_cnt)

    return graphs, ys
