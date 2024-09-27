import argparse
import logging
import random
import time

import numpy as np
import toml
import torch

from learner.dataset.dataset import ALL_KEY, Dataset
from learner.dataset.dataset_factory import dataset_class, dataset_options
from learner.deep_learning.gnn_model import DeepLearningModel
from learner.deep_learning.nn import RGnn
from learner.deep_learning.nn.loss_factory import get_loss_function
from learner.deep_learning.nn.train_info import GnnTrainInfo
from learner.deep_learning.nn.training import evaluate, train, train_ranker
from learner.deep_learning.representation.gnn_representation import GnnRepresentation
from learner.evaluation_info import EvaluationInfo
from learner.feature_generation.estimator.estimator_factory import check_estimator_compatibility, get_estimator
from learner.feature_generation.representation.wlf_representation import CCwlRepresentation
from learner.feature_generation.wlf_model import FeatureGenerationModel
from learner.understand import understand_model
from util.statistics import dump_several_stats, print_mat

logging.basicConfig(
    level=logging.INFO,
    format="%(levelname)s [%(filename)s:%(lineno)s] %(message)s",
)


def parse_args_from_command_line():
    # fmt: off
    parser = argparse.ArgumentParser()
    parser.add_argument("model_config")
    parser.add_argument("data_config")
    parser.add_argument("-s", "--save_file", type=str, default=None)
    parser.add_argument("--seed", default=0, type=int)
    parser.add_argument("--device", type=int, default=None, help="gpu id")
    parser.add_argument("--debug", action="store_true")
    parser.add_argument("--understand", default=None, help="descriptor for output dir")
    args = parser.parse_args()
    # fmt: on

    return parse_args_from_toml(
        model_config=args.model_config,
        data_config=args.data_config,
        seed=args.seed,
        save_file=args.save_file,
        device=args.device,
        debug=args.debug,
        understand=args.understand,
    )


def parse_args_from_toml(model_config, data_config, **kwargs):
    model_config = toml.load(model_config)
    data_config = toml.load(data_config)

    def p(name, default, group, choices=None):
        if name in kwargs:  # kwargs overrides toml file configs
            return kwargs[name]
        if group in model_config and name in model_config[group]:
            arg = model_config[group][name]
            if choices is not None and arg not in choices:
                log = f"Invalid value for {name}: {arg}.\nChoose from {choices}"
                raise ValueError(log)
            return model_config[group][name]
        return default

    model_method = model_config["model"]["method"]

    opts = argparse.Namespace(
        domain_pddl=data_config["training"]["domain_pddl"],
        tasks_dir=data_config["training"]["tasks_dir"],
        plans_dir=data_config["training"]["plans_dir"],
        numeric=data_config["training"]["numeric"],
        save_file=kwargs["save_file"],
        seed=kwargs["seed"],
        device=kwargs["device"] if "device" in kwargs else None,
        debug=kwargs["debug"] if "debug" in kwargs else False,
        model_method=model_method,
        ## general data options
        val_ratio=p("val_ratio", 0.0, group="data"),
        static_facts=p("static_facts", False, group="data", choices=[True, False]),
        static_fluents=p("static_fluents", False, group="data", choices=[True, False]),
        ## general model and estimator options
        target=p("target", "h", group="model", choices=dataset_options()),
        round=p("round", True, group="estimator"),
        schemata_strategy=p(
            "schemata_strategy",
            "all",
            group="estimator",
            choices=["all", "each", "pair", "subset"],
        ),
        understand=kwargs["understand"] if "understand" in kwargs else None,
    )

    if model_method == "gnn":
        opts.numeric_agnostic = p("numeric_agnostic", False, group="representation")
        opts.dynamic_features = p("dynamic_features", False, group="representation")

        opts.jumping_knowledge = p("jumping_knowledge", False, group="estimator")
        opts.nhid = p("nhid", 64, group="estimator")
        opts.nlayers = p("nlayers", 4, group="estimator")
        opts.aggr = p("aggr", "mean", group="estimator")
        opts.pool = p("pool", "sum", group="estimator")

        opts.lr = p("lr", 0.01, group="opt")
        opts.l2 = p("l2", 0.01, group="opt")
        opts.patience = p("patience", 10, group="opt")
        opts.reduction = p("reduction", 0.1, group="opt")
        opts.epochs = p("epochs", 5000, group="opt")
        opts.batch_size = p("batch_size", 16, group="opt")

        if opts.target in {"p", "d"}:
            opts.round = True

        if opts.understand:
            print(f"WARNING: --understand does nothing for GNNs.")
    elif model_method == "wlf":
        opts.cat_iterations = p("cat_iterations", 2, group="representation")
        opts.con_iterations = p("con_iterations", 2, group="representation")

        opts.estimator_name = p("estimator", "gpr", group="estimator")

        check_estimator_compatibility(opts)
    else:
        raise ValueError(f"Invalid model method: {model_method}")

    if opts.schemata_strategy != "all" and opts.target == "h":
        raise NotImplementedError("TODO: determine a canonical set. e.g. min/max")
    if opts.target == "r" and opts.val_ratio != 0.0:
        raise NotImplementedError("Validation set not supported for rankers")

    ### dump args
    print("Configuration:")
    print_mat([[k, v] for k, v in vars(opts).items()])

    return opts


def train_wlf(opts):
    debug = opts.debug

    ### dataset
    representation = CCwlRepresentation(opts)
    dataset = dataset_class(opts.target)(opts, representation)
    schemata_keys = dataset.schemata_keys
    X_tr, X_val, y_tr_true, y_val_true = dataset.get_dataset_split()
    representation.dump()

    ### init model
    estimator = get_estimator(opts)

    ### wrap everything into a single object
    model = FeatureGenerationModel()
    model.set_objects(opts=opts, representation=representation, estimator=estimator)
    model.update_estimator_from_dataset(dataset)

    ### main training code
    training_times = model.fit(X_tr, y_tr_true)

    ### evaluation
    metrics = dataset.get_metrics()
    train_scores = {k: {metric: "na" for metric in metrics} for k in schemata_keys}
    val_scores = {k: {metric: "na" for metric in metrics} for k in schemata_keys}
    for X, y_true, desc in [(X_tr, y_tr_true, "train"), (X_val, y_val_true, "val")]:
        if len(X) == 0:
            continue
        for k in schemata_keys:
            y_pred = model.predict(X, k)
            scores = {metric: f(y_true[k], y_pred) for metric, f in metrics.items()}
            if desc == "train":
                train_scores[k] = scores
            else:
                val_scores[k] = scores
    train_info = EvaluationInfo(training_times, train_scores, val_scores)
    train_info.dump()

    ### number of features
    n_features = X_tr.shape[1]
    print(f"Number of computed features: {n_features}")
    # unique_X = np.unique(X_tr, axis=1)
    # n_unique_features = unique_X.shape[1]
    # print(f"Number of unique features: {n_unique_features}")

    ### save model
    print(end="", flush=True)
    model.train_info = train_info
    if opts.save_file is not None:
        model.save(opts.save_file)

    ### understand features:
    if opts.understand:
        if len(X_val) == 0:
            X = X_tr
        else:
            X = np.vstack([X_tr, X_val])
        understand_model(model, opts.understand, np.vstack(X))

    return model


def train_gnn(opts):
    debug = opts.debug

    ### cuda
    if torch.cuda.is_available() and opts.device is not None:
        device = f"cuda:{opts.device}"
    elif torch.cuda.is_available() and opts.device is None:
        device = "cuda"
    else:
        device = "cpu"
    device = torch.device(device)
    print(f"Device: {device}")
    if device.type == "cuda":
        device_name = torch.cuda.get_device_name()
        device_number = torch.cuda.current_device()
        print(f"Hardware:")
        print(f"Device name: {device_name}")
        print(f"Device number: {device_number}")

    ### dataset
    representation = GnnRepresentation(opts)
    dataset: Dataset = dataset_class(opts.target)(opts, representation)
    schemata_keys = sorted(dataset.schemata_keys)
    if opts.target == "r":
        train_groups = dataset.dataset
    else:
        dump_several_stats(
            ([g.num_nodes for g in dataset], "n_nodes"),
            ([sum(e.shape[1] for e in g.edge_index) for g in dataset], "n_edges"),
        )
        train_loader, val_loader = dataset.get_loaders(opts, device)
    representation.dump()

    ### init model
    gnn = RGnn(
        in_dim=representation.n_node_features,
        out_dim={"h": 1, "d": 1, "r": 1, "p": len(schemata_keys)}[opts.target],
        n_edge_labels=representation.n_edge_labels,
        opts=opts,
    ).to(device)
    # https://pytorch-geometric.readthedocs.io/en/latest/tutorial/compile.html
    torch.compile(gnn, dynamic=True)
    print("num_parameters:", gnn.num_parameters)

    ### wrap everything into a single object
    model = DeepLearningModel()
    model.set_objects(opts=opts, representation=representation, model=gnn)

    ### init optimiser
    loss_fn = get_loss_function(opts)
    optimiser = torch.optim.Adam(gnn.parameters(), lr=opts.lr, weight_decay=opts.l2)
    scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(
        optimiser, mode="min", factor=opts.reduction, patience=opts.patience
    )

    ### main training loop
    print("Training started.")
    train_info = GnnTrainInfo()
    best_weights = None
    prev_lr = opts.lr
    start_t = time.time()

    try:
        for e in range(1, opts.epochs + 1):
            t = time.time()

            if opts.target == "r":  # ranking trains on entire dataset
                tr_out = train_ranker(
                    model, device, train_groups, loss_fn, optimiser, opts.batch_size
                )
                tr_loss = tr_out["loss"]
                va_loss = tr_loss
            else:
                tr_out = train(model, device, train_loader, loss_fn, optimiser, debug)
                tr_loss = tr_out["loss"]
                va_out = evaluate(model, device, val_loader, loss_fn, debug)
                va_loss = va_out["loss"]
            scheduler.step(va_loss)

            # take model weights corresponding to best combined metric
            l = opts.val_ratio
            combined_loss = l * tr_loss + (1 - l) * va_loss
            train_info.train_curve.append(tr_loss)
            train_info.val_curve.append(va_loss)
            if combined_loss < train_info.best_combined_loss:
                train_info.best_epoch = e
                train_info.best_combined_loss = combined_loss
                train_info.best_train_loss = tr_loss
                train_info.best_val_loss = va_loss
                best_weights = model.nn.state_dict()

            # fmt: off
            if opts.target == "r":
                print(", ".join([
                    f"epoch {e}",
                    f"time {time.time() - t}",
                    f"train_loss {tr_loss}",
                ]), flush=True)
            else:
                print(", ".join([
                    f"epoch {e}",
                    f"time {time.time() - t}",
                    f"train_loss {tr_loss}",
                    f"val_loss {va_loss}",
                    f"combined_loss {combined_loss}",
                ]), flush=True)
            # fmt: on

            lr = scheduler.get_last_lr()[0]
            if lr < prev_lr:
                print(f"lr decreased from {prev_lr:.2E} to {lr:.2E}")
            if lr < 1e-5:
                print(f"Stopping early due to small lr: {lr:.2E}")
                break
            prev_lr = lr
    except KeyboardInterrupt:
        print("Stopping early due to keyboard interrupt")

    print("Training completed!")
    train_info.training_time = time.time() - start_t
    train_info.dump()

    ### evaluation
    if opts.target != "r":
        metrics = dataset.get_metrics()
        train_scores = {k: {metric: "na" for metric in metrics} for k in schemata_keys}
        val_scores = {k: {metric: "na" for metric in metrics} for k in schemata_keys}
        for loader, desc in [(train_loader, "train"), (val_loader, "val")]:
            y_pred = {k: [] for k in schemata_keys}
            y_true = {k: [] for k in schemata_keys}
            for data in loader:
                data = data.to(device)
                pred = gnn.forward(data)
                if opts.round:
                    pred = torch.round(pred)
                if opts.target in {"h", "d"}:
                    y_true[ALL_KEY].append(data.y.detach().cpu().numpy())
                    y_pred[ALL_KEY].append(pred.detach().cpu().numpy())
                elif opts.target == "p":
                    for i, k in sorted(enumerate(schemata_keys)):
                        y_true[k].append(data.y[:, i].detach().cpu().numpy())
                        y_pred[k].append(pred[:, i].detach().cpu().numpy())
            y_pred = {k: np.concatenate(y_pred[k]) for k in schemata_keys}
            y_true = {k: np.concatenate(y_true[k]) for k in schemata_keys}
            for k in schemata_keys:
                scores = {m: f(y_true[k], y_pred[k]) for m, f in metrics.items()}
                if desc == "train":
                    train_scores[k] = scores
                else:
                    val_scores[k] = scores
        train_info = EvaluationInfo(None, train_scores, val_scores)
        train_info.dump()
        model.train_info = train_info

    ### save model
    print(end="", flush=True)
    if opts.save_file is not None:
        model.nn.load_state_dict(best_weights)
        model.save(opts.save_file)

    return model


if __name__ == "__main__":
    args = parse_args_from_command_line()

    ## seed
    seed = args.seed
    random.seed(seed)
    np.random.seed(seed)
    torch.manual_seed(seed)
    torch.cuda.manual_seed(seed)

    if args.model_method == "gnn":
        train_gnn(args)
    elif args.model_method == "wlf":
        train_wlf(args)
