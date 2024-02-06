import random
from torch_geometric.data import Data
from torch_geometric.loader import DataLoader
from sklearn.model_selection import train_test_split
from util.stats import get_stats
from representation import REPRESENTATIONS
from .factory import state_cost_dataset_from_plans, ALL_KEY


def get_tensor_graphs_from_plans(args):
    print("Generating graphs from plans...")
    graphs = []

    representation = args.rep
    domain_pddl = args.domain_pddl

    for problem_pddl, plan in state_cost_dataset_from_plans(
        domain_pddl,
        args.tasks_dir,
        args.plans_dir,
        args,
    ).items():
        rep = REPRESENTATIONS[representation](
            domain_pddl=domain_pddl,
            problem_pddl=problem_pddl,
        )
        rep.convert_to_pyg()

        for state, schema_cnt in plan:
            state = rep.str_to_state(state)
            x, edge_index = rep.state_to_tgraph(state)
            y = schema_cnt[ALL_KEY]
            graph = Data(x=x, edge_index=edge_index, y=y)
            graphs.append(graph)

    print("Graphs generated!")
    return graphs


def get_loaders_from_args_gnn(args):
    batch_size = args.batch_size
    small_train = args.small_train

    dataset = get_tensor_graphs_from_plans(args)
    if small_train:
        random.seed(123)
        dataset = random.sample(dataset, k=1000)

    trainset, valset = train_test_split(
        dataset, test_size=0.15, random_state=4550
    )

    get_stats(dataset=dataset, desc="Whole dataset")
    get_stats(dataset=trainset, desc="Train set")
    get_stats(dataset=valset, desc="Val set")
    print("train size:", len(trainset))
    print("validation size:", len(valset))

    train_loader = DataLoader(trainset, batch_size=batch_size, shuffle=True)
    val_loader = DataLoader(valset, batch_size=batch_size, shuffle=False)

    return train_loader, val_loader
