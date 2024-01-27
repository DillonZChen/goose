import os

""" Module for reading information from logs. """


def predict_finished_correctly(f):
    log = open(f, "r").read()
    finished_correctly = "Initial heuristic value" in log
    return finished_correctly


def search_finished_correctly(f):
    log = open(f, "r").read()
    finished_correctly = (
        "timed out after" in log
        or "Solution found." in log
        or "Time limit has been reached." in log
    )
    return finished_correctly


def scrape_search_log(file):
    """assumes fast downward like log files"""

    stats = {
        "first_h": -1,
        "solved": 0,
        "time": -1,
        "cost": -1,
        "expanded": -1,
        "evaluated": -1,
        "seen_colours": -1,  # for the wl methods only
        "unseen_colours": -1,
        "ratio_seen_colours": -1,
        "std": -1,
        "tried": 0,
    }

    if not os.path.exists(file):
        return stats

    for line in open(file, "r").readlines():
        line = line.replace(" state(s).", "")
        toks = line.split()
        if len(toks) == 0:
            continue

        if "Solution found." in line:
            stats["solved"] = 1
        elif "Search time:" in line:
            stats["time"] = float(toks[-1].replace("s", ""))
        elif "Plan cost:" in line:
            stats["cost"] = int(toks[-1])
        elif len(toks) >= 2 and "Expanded" == toks[-2]:
            stats["expanded"] = int(toks[-1])
        elif len(toks) >= 2 and "Evaluated" == toks[-2]:
            stats["evaluated"] = int(toks[-1])
        elif "Initial heuristic value" in line:
            stats["first_h"] = int(toks[-1])
        elif "seen/unseen colours in itr" in line:
            stats["seen_colours"] += int(toks[-2])
            stats["unseen_colours"] += int(toks[-1])
        elif "Computed std at initial state:" in line:
            stats["std"] = float(toks[-1])

    if stats["seen_colours"] != -1 and stats["unseen_colours"] != -1:
        stats["seen_colours"] += 1
        stats["unseen_colours"] += 1
        stats["ratio_seen_colours"] = stats["seen_colours"] / (
            stats["seen_colours"] + stats["unseen_colours"]
        )

    if stats["time"] > 1800:  # assume timeout is 1800
        stats["solved"] = 0
        stats["time"] = 1800

    stats["tried"] = 1

    return stats


def scrape_train_log(file):
    stats = {
        "epochs": -1,
        "time": 0,
        "model_path": -1,
        "best_avg_loss": float("inf"),
    }

    model_time = False
    arguments = False

    for line in open(file, "r").readlines():
        line = line.replace(",", "")
        toks = line.split()
        if len(toks) == 0:
            continue

        if "Loading " in line:
            arguments = False
        if arguments:
            if len(toks) == 1:
                stats[toks[0]] = ""
            else:
                assert len(toks) == 2
                stats[toks[0]] = toks[1]
        if "Parsed arguments" in line:
            arguments = True

        if toks[0] == "epoch":
            stats["epochs"] = int(toks[1])
            stats["time"] += float(toks[-1])

        if model_time:
            stats["model_path"] = line.replace("\n", "")
            model_time = False
        if "Model parameter file" in line:
            model_time = True
        if toks[0] == "best_avg_loss":
            stats["best_avg_loss"] = float(toks[1])

    return stats


def scrape_kernel_train_log(log_file):
    assert os.path.exists(log_file), log_file
    stats = {}
    lines = list(open(log_file, "r", encoding="ISO-8859-1").readlines())
    for line in lines:
        try:
            toks = line.split()
            if "train_mse" in line:
                stats["train_mse"] = float(toks[-1])
            elif "train_f1_macro" in line:
                stats["train_f1"] = float(toks[-1])
            elif "val_mse" in line:
                stats["val_mse"] = float(toks[-1])
            elif "val_f1_macro" in line:
                stats["val_f1"] = float(toks[-1])
            elif "zero_weights" in line:
                weights = int(toks[1].split("/")[1])
                zeros = int(toks[1].split("/")[0])
                stats["nonzero_weights"] = weights - zeros
            elif "Model training completed in " in line:
                stats["time"] = float(toks[-1].replace("s", ""))
        except:
            pass

    if "nonzero_weights" not in stats:
        stats["nonzero_weights"] = "na"

    return stats
