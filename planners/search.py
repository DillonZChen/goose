import os

# TODO refactor whole code base
LINEAR_MODELS = {
    "gpr",
    "linear-svr",
    "linear-regression",
    "ridge",
    "lasso",
    "mip",
}

_DOWNWARD_CPU = "./planners/downward_cpu/fast-downward.py"
_DOWNWARD_GPU = "./planners/downward_gpu/fast-downward.py"


def search_cmd(args, model_type):
    if model_type == "wl":
        get_search_cmd = fd_wl
    elif model_type == "gnn":
        get_search_cmd = fd_gnn
    else:
        raise ValueError(f"Unknown model type: {model_type}")

    aux_file = args.aux_file
    plan_file = args.plan_file

    description = repr(hash(repr(args))).replace("-", "n")

    if aux_file is None:
        os.makedirs("aux", exist_ok=True)
        aux_file = f"aux/{description}.aux"

    if plan_file is None:
        os.makedirs("plans", exist_ok=True)
        plan_file = f"plans/{description}.plan"

    cmd = get_search_cmd(args, aux_file, plan_file)
    cmd = f"export GOOSE={os.getcwd()}/learner && {cmd}"
    return cmd, aux_file


def fd_wl(args, aux_file, plan_file):
    mf = os.path.abspath(args.model_path)
    df = os.path.abspath(args.domain_pddl)
    pf = os.path.abspath(args.problem_pddl)
    algorithm = args.algorithm

    from learner.models.save_load import load_kernel_model

    model = load_kernel_model(mf)
    # print(model.get_weights())
    # exit(-1)

    if args.pybind or model.model_name not in LINEAR_MODELS:
        model_type = "kernel_model"
    elif model.model_name == "xgb":
        model_type = "xgboost_model"
    else:
        model_type = "linear_model"

    if args.train:
        model_type += "_online"
    h_goose = f'{model_type}(model_file="{mf}", domain_file="{df}", instance_file="{pf}")'
    if args.std:
        assert model_type == "linear_model"
        h_goose = f'{model_type}(model_file="{mf}", domain_file="{df}", instance_file="{pf}", compute_std=true)'

    fd_search = ""
    if algorithm in {"lazy", "eager"}:
        fd_search = f"{algorithm}_greedy([{h_goose}])"
    elif algorithm == "mq":
        fd_search = f"mq_goose([{h_goose}], symmetry=false)"
    elif algorithm == "mqp":
        fd_search = f"mq_goose([{h_goose}], symmetry=true)"
    # elif algorithm == "lama":
    #     h_ff = "ff(transform=adapt_costs(one))"
    #     h_lm = "landmark_sum(lm_factory=lm_reasonable_orders_hps(lm_rhw()),transform=adapt_costs(one),pref=false)"
    #     fd_search = f"lazy_greedy([{h_goose},{h_lm}],preferred=[],cost_type=one,reopen_closed=false)"

    cmd = f"{_DOWNWARD_CPU} --sas-file {aux_file} --plan-file {plan_file} {df} {pf} --search '{fd_search}'"

    return cmd



def fd_gnn(args, aux_file, plan_file):
    mf = os.path.abspath(args.model_path)
    df = os.path.abspath(args.domain_pddl)
    pf = os.path.abspath(args.problem_pddl)
    algorithm = args.algorithm

    h_goose = f'goose(model_path="{mf}", domain_file="{df}", instance_file="{pf}")'

    fd_search = ""
    if algorithm == "eager":
        fd_search = f"batch_eager_greedy([{h_goose}])"
    else:
        raise ValueError(f"Unknown algorithm: {algorithm}")

    cmd = f"{_DOWNWARD_GPU} --sas-file {aux_file} --plan-file {plan_file} {df} {pf} --search '{fd_search}'"

    return cmd
