from representation import REPRESENTATIONS
from .factory import state_cost_dataset_from_plans, ALL_KEY


def get_graphs_from_plans(args):
    print("Generating graphs from plans...")
    dataset = []  # can probably make a class for this

    schema_keys = set()

    representation = args.rep
    domain_pddl = args.domain_pddl

    for problem_pddl, plan in state_cost_dataset_from_plans(
        domain_pddl,
        args.tasks_dir,
        args.plans_dir,
    ).items():
        rep = REPRESENTATIONS[representation](
            domain_pddl=domain_pddl,
            problem_pddl=problem_pddl,
        )

        for state, schema_cnt in plan:
            state = rep.str_to_state(state)
            graph = rep.state_to_cgraph(state)
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
