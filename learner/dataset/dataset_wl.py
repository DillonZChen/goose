import sys

sys.path.append("..")
import os
from tqdm import tqdm
from representation import REPRESENTATIONS
from .dataset_factory import collect_states_from_plan, ALL_KEY

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

        plan = collect_states_from_plan(domain_pddl, problem_pddl, plan_file, args)

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
