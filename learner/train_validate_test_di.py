"""
Main script for running *domain-independent* GOOSE experiments described in AAAI-24. The experiment pipeline consists of
1. training 5 models for each (graph_representation, GNN) configuration
2. validating the 5 models for each domain with search
3. selecting the best model from the 5 models
4. evaluating the best model on test problems
"""
import os
import argparse
import numpy as np
from itertools import product
from dataset.goose_domain_info import GOOSE_DOMAINS
from representation import REPRESENTATIONS
from util.scrape_log import scrape_search_log, scrape_train_log, search_finished_correctly
from util.search import VAL_REPEATS, FAIL_LIMIT, fd_cmd, sorted_nicely

_SEARCH = "gbbfs"

_TRAINED_MODEL_DIR = "all_trained_models"
_VALIDATED_MODEL_DIR = "aaai24_trained_models"

_MAIN_LOG_DIR = f"aaai24_logs"
_LOG_DIR_TRAIN = f"{_MAIN_LOG_DIR}/train"
_LOG_DIR_VAL = f"{_MAIN_LOG_DIR}/val"
_LOG_DIR_SELECT = f"{_MAIN_LOG_DIR}/select"
_LOG_DIR_TEST = f"{_MAIN_LOG_DIR}/test"

os.makedirs(_TRAINED_MODEL_DIR, exist_ok=True)
os.makedirs(_VALIDATED_MODEL_DIR, exist_ok=True)
os.makedirs(_MAIN_LOG_DIR, exist_ok=True)
os.makedirs(_LOG_DIR_TRAIN, exist_ok=True)
os.makedirs(_LOG_DIR_VAL, exist_ok=True)
os.makedirs(_LOG_DIR_SELECT, exist_ok=True)
os.makedirs(_LOG_DIR_TEST, exist_ok=True)


def get_model_name(rep, L, H, aggr, patience, val_repeat):
    return f"di_{rep}_L{L}_H{H}_{aggr}_p{patience}_v{val_repeat}"


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-r", "--representation", required=True, choices=REPRESENTATIONS)
    args = parser.parse_args()

    rep = args.representation

    patience = 20
    H = 64
    Ls = [4, 8, 12, 16]
    aggrs = ["mean", "max"]

    for L, aggr in product(Ls, aggrs):
        #######################################################################
        """train"""
        for val_repeat in range(VAL_REPEATS):
            os.system("date")

            model_name = get_model_name(rep, L, H, aggr, patience, val_repeat)
            model_file = f"{_TRAINED_MODEL_DIR}/{model_name}.dt"
            train_log_file = (
                f"{_LOG_DIR_TRAIN}/di_{rep}_L{L}_H{H}_{aggr}_p{patience}_v{val_repeat}.log"
            )

            if not os.path.exists(model_file) or not os.path.exists(train_log_file):
                cmd = f"python3 train_gnn.py ipc -r {rep} -L {L} -H {H} --aggr {aggr} --patience {patience} --save-file {model_file}"
                os.system(f"echo training with ipc {rep}, see {train_log_file}")
                os.system(f"{cmd} > {train_log_file}")
            else:
                os.system(f"echo already trained for ipc {rep}, see {train_log_file}")
        #######################################################################

    for L, aggr, domain in product(Ls, aggrs, GOOSE_DOMAINS):
        val_dir = f"../dataset/goose/{domain}/val"
        test_dir = f"../dataset/goose/{domain}/test"
        df = f"../dataset/goose/{domain}/domain.pddl"  # domain file
        #######################################################################
        """validate"""
        for val_repeat in range(VAL_REPEATS):
            model_name = get_model_name(rep, L, H, aggr, patience, val_repeat)
            model_file = f"{_TRAINED_MODEL_DIR}/{model_name}.dt"
            assert os.path.exists(model_file)

            for f in os.listdir(val_dir):
                os.system("date")

                val_log_file = f"{_LOG_DIR_VAL}/{f.replace('.pddl', '')}_{domain}_{model_name}.log"

                finished_correctly = False
                if os.path.exists(val_log_file):
                    finished_correctly = search_finished_correctly(val_log_file)
                if not finished_correctly:
                    pf = f"{val_dir}/{f}"
                    cmd, intermediate_file = fd_cmd(df=df, pf=pf, m=model_file, search=_SEARCH)
                    os.system(f"echo validating with {domain} {rep}, see {val_log_file}")
                    os.system(f"{cmd} > {val_log_file}")
                    if os.path.exists(intermediate_file):
                        os.remove(intermediate_file)
                else:
                    os.system(f"echo already validated for {domain} {rep}, see {val_log_file}")
        #######################################################################

        """ selection """
        # after running all validation repeats, we pick the best one
        best_model = -1
        best_solved = 0
        best_expansions = float("inf")
        best_runtimes = float("inf")
        best_loss = float("inf")
        best_train_time = float("inf")

        # see if any model solved anything
        for val_repeat in range(VAL_REPEATS):
            model_name = get_model_name(rep, L, H, aggr, patience, val_repeat)

            solved = 0
            for f in os.listdir(val_dir):
                val_log_file = f"{_LOG_DIR_VAL}/{f.replace('.pddl', '')}_{domain}_{model_name}.log"
                stats = scrape_search_log(val_log_file)
                solved += stats["solved"]
            best_solved = max(best_solved, solved)

        # break ties
        for val_repeat in range(VAL_REPEATS):
            model_name = get_model_name(rep, L, H, aggr, patience, val_repeat)
            model_file = f"{_TRAINED_MODEL_DIR}/{model_name}.dt"

            solved = 0
            expansions = []
            runtimes = []
            for f in os.listdir(val_dir):
                val_log_file = f"{_LOG_DIR_VAL}/{f.replace('.pddl', '')}_{domain}_{model_name}.log"
                stats = scrape_search_log(val_log_file)
                solved += stats["solved"]
                if stats["solved"]:
                    expansions.append(stats["expanded"])
                    runtimes.append(stats["time"])
            expansions = np.median(expansions) if len(expansions) > 0 else -1
            runtimes = np.median(runtimes) if len(runtimes) > 0 else -1
            train_log_file = (
                f"{_LOG_DIR_TRAIN}/di_{rep}_L{L}_H{H}_{aggr}_p{patience}_v{val_repeat}.log"
            )
            train_stats = scrape_train_log(train_log_file)
            avg_loss = train_stats["best_avg_loss"]
            train_time = train_stats["time"]
            # choose best model
            if (solved == best_solved and best_solved > 0 and expansions < best_expansions) or (
                solved == best_solved and best_solved == 0 and avg_loss < best_loss
            ):
                best_model = model_file
                best_expansions = expansions
                best_runtimes = runtimes
                best_loss = avg_loss
                best_train_time = train_time

        # log best model stats
        desc = f"di_{rep}_{domain}_L{L}_H{H}_{aggr}_p{patience}"
        best_model_file = f"{_VALIDATED_MODEL_DIR}/{desc}.dt"
        with open(f"{_LOG_DIR_SELECT}/{desc}.log", "w") as f:
            f.write(f"model: {best_model}\n")
            f.write(f"solved: {best_solved} / {len(os.listdir(val_dir))}\n")
            f.write(f"median_expansions: {best_expansions}\n")
            f.write(f"median_runtime: {best_runtimes}\n")
            f.write(f"avg_loss: {best_loss}\n")
            f.write(f"train_time: {best_train_time}\n")
            f.close()
        os.system(f"cp {best_model} {best_model_file}")
        #######################################################################

        """ test """
        failed = 0

        # warmup first
        f = sorted_nicely(os.listdir(test_dir))[0]
        pf = f"{test_dir}/{f}"
        cmd, intermediate_file = fd_cmd(df=df, pf=pf, m=best_model_file, search=_SEARCH, timeout=30)
        os.system("date")
        os.system(f"echo warming up with {domain} {rep} {f.replace('.pddl', '')} {best_model_file}")
        os.popen(cmd).readlines()
        try:
            os.remove(intermediate_file)
        except OSError:
            pass

        # test on problems
        for f in sorted_nicely(os.listdir(test_dir)):
            os.system("date")
            test_log_file = f"{_LOG_DIR_TEST}/{f.replace('.pddl', '')}_{desc}.log"
            finished_correctly = False
            if os.path.exists(test_log_file):
                finished_correctly = search_finished_correctly(test_log_file)
            if not finished_correctly:
                pf = f"{test_dir}/{f}"
                cmd, intermediate_file = fd_cmd(df=df, pf=pf, m=best_model_file, search=_SEARCH)
                os.system(f"echo testing {domain} {rep}, see {test_log_file}")
                os.system(f"{cmd} > {test_log_file}")
                if os.path.exists(intermediate_file):
                    os.remove(intermediate_file)
            else:
                os.system(f"echo already tested for {domain} {rep}, see {test_log_file}")

            # check if failed or not
            assert os.path.exists(test_log_file)
            log = open(test_log_file, "r").read()
            solved = "Solution found." in log
            if solved:
                failed = 0
                print("solved", flush=True)
            else:
                failed += 1
                print("failed", flush=True)
            if failed >= FAIL_LIMIT[domain]:
                break
        #######################################################################
