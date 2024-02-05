import os
import random
from tqdm import tqdm
from torch_geometric.data import Data
from torch_geometric.loader import DataLoader
from sklearn.model_selection import train_test_split
from util.stats import get_stats
from representation import REPRESENTATIONS

_DOWNWARD = "./../planners/downward_cpu/fast-downward.py"
_POWERLIFTED = "./../planners/powerlifted/powerlifted.py"


def get_plan_info(domain_pddl, problem_pddl, plan_file, args):
    planner = args.planner

    states = []
    actions = []

    with open(plan_file, "r") as f:
        for line in f.readlines():
            if ";" in line:
                continue
            actions.append(line.replace("\n", ""))

    aux_garbage = repr(
        hash((domain_pddl, problem_pddl, plan_file, repr(args)))
    )
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

    if not os.path.exists(state_output_file):
        err_msg = f"Failed to generate states from training data plans. This may be because you did not build the planners yet. This can be done with\n\n\tsh build_components.sh\n"
        raise RuntimeError(err_msg)
    
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


def get_tensor_graphs_from_plans(args):
    print("Generating graphs from plans...")
    graphs = []

    representation = args.rep
    domain_pddl = args.domain_pddl
    tasks_dir = args.tasks_dir
    plans_dir = args.plans_dir

    for plan_file in tqdm(sorted(list(os.listdir(plans_dir)))):
        problem_pddl = f"{tasks_dir}/{plan_file.replace('.plan', '.pddl')}"
        assert os.path.exists(problem_pddl), problem_pddl
        plan_file = f"{plans_dir}/{plan_file}"
        rep = REPRESENTATIONS[representation](
            domain_pddl=domain_pddl, problem_pddl=problem_pddl
        )
        rep.convert_to_pyg()
        plan = get_plan_info(domain_pddl, problem_pddl, plan_file, args)

        for state, schema_cnt in plan:
            state = rep.str_to_state(state)
            x, edge_index = rep.state_to_tgraph(state)
            y = sum(schema_cnt.values())
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
