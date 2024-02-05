import unittest
import os
import sys

_TOL = 1e-6


def in_venv():
    return sys.prefix != sys.base_prefix


def log_output(output, file):
    with open(file, "w") as f:
        for line in output:
            f.write(line)
        f.close()
    return


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


class TestWlf(unittest.TestCase):
    def test_wlf_ilg_gpr(self):
        # train
        print("========= Training =========")
        cmd = "python3 train.py experiments/models/wlf_ilg_gpr.toml experiments/ipc23-learning/blocksworld.toml --save-file tests/test.model"

        output = list(os.popen(cmd).readlines())

        mse_train = None
        mse_val = None
        f1_train = None
        f1_val = None
        model_saved = False

        for line in output:
            toks = line.split()
            if len(toks) > 0 and toks[0] == "mse":
                mse_train = float(toks[2])
                mse_val = float(toks[3])
            elif len(toks) > 0 and toks[0] == "f1_macro":
                f1_train = float(toks[2])
                f1_val = float(toks[3])
            elif "Model saved!" in line:
                model_saved = True

        log_output(output, "tests/train_wlf.test.log")

        self.assertTrue(mse_train is not None)
        self.assertTrue(mse_val is not None)
        self.assertTrue(f1_train is not None)
        self.assertTrue(f1_val is not None)

        self.assertTrue(model_saved)

        self.assertTrue(mse_train < _TOL, f"mse_train: {mse_train}")
        self.assertTrue(mse_val < 1.0, f"mse_val: {mse_val}")
        self.assertTrue(f1_train > 1 - _TOL, f"f1_train: {f1_train}")
        self.assertTrue(f1_val > 0.5, f"f1_val: {f1_val}")

        # run
        cmd_cpp = "python3 run_wlf.py benchmarks/ipc23-learning/blocksworld/domain.pddl benchmarks/ipc23-learning/blocksworld/testing/medium/p01.pddl tests/test.model"
        cmd_py = cmd_cpp + " --pybind"

        print("========= Executing search with cpp =========")
        output_cpp = list(os.popen(cmd_cpp).readlines())
        print("========= Executing search with py =========")
        output_py = list(os.popen(cmd_py).readlines())

        cpp_f = "tests/run_wlf_cpp.test.log"
        py_f = "tests/run_wlf_py.test.log"
        log_output(output_cpp, cpp_f)
        log_output(output_py, py_f)

        cpp_stats = scrape_search_log(cpp_f)
        py_stats = scrape_search_log(py_f)

        for key in ["cost", "expanded", "evaluated", "ratio_seen_colours"]:
            cpp_val = cpp_stats[key]
            py_val = py_stats[key]
            self.assertTrue(
                cpp_val == py_val, f"cpp_{key}: {cpp_val}, py_{key}: {py_val}"
            )


if __name__ == "__main__":
    if not in_venv():
        print("Not in virtual environment. Exiting...")
        exit(1)
    unittest.main()
