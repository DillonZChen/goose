import unittest
import os
from util import *


class TestWlf(unittest.TestCase):
    def test_ilg_gpr(self):
        for domain in [
            "blocksworld",
            "spanner",  # due to numerical instability, test fails
        ]:
            with self.subTest(domain):
                self.wlf_helper_test("wlf_ilg_gpr", domain)

    def wlf_helper_test(self, model_config, domain):
        desc = f"{domain}-{model_config}"
        config_file = f"experiments/models/{model_config}.toml"
        data_file = f"experiments/ipc23-learning/{domain}.toml"
        assert os.path.exists(config_file), config_file
        assert os.path.exists(data_file), data_file

        print(f"========= Running unit tests for {desc} =========")

        # train
        print("=== Training ===")
        cmd = f"python3 train.py {config_file} {data_file} --save_file tests/wlf_{desc}.model"
        log_output, err_output = get_output(cmd)

        mse_train = None
        mse_val = None
        f1_train = None
        f1_val = None
        model_saved = False

        for line in log_output:
            toks = line.split()
            if len(toks) > 0 and toks[0] == "mse":
                mse_train = float(toks[2])
                mse_val = float(toks[3])
            elif len(toks) > 0 and toks[0] == "f1_macro":
                f1_train = float(toks[2])
                f1_val = float(toks[3])
            elif "Model saved!" in line:
                model_saved = True

        f = f"tests/train_wlf_{desc}.log"
        log((log_output, err_output), f)

        if (
            mse_train is None
            or mse_val is None
            or f1_train is None
            or f1_val is None
        ):
            self.assertTrue(False, "\n".join(err_output))

        self.assertTrue(model_saved)

        print(f"mse_train: {mse_train}")
        print(f"mse_val: {mse_val}")
        print(f"f1_train: {f1_train}")
        print(f"f1_val: {f1_val}")

        # run
        cmd_cpp = f"python3 run_wlf.py benchmarks/ipc23-learning/{domain}/domain.pddl benchmarks/ipc23-learning/{domain}/testing/medium/p01.pddl tests/wlf_{desc}.model"
        cmd_py = cmd_cpp + " --pybind"

        cpp_f = f"tests/run_wlf_cpp_{desc}.log"
        py_f = f"tests/run_wlf_py_{desc}.log"

        print("=== Executing search with cpp ===")
        output_cpp = get_output(cmd_cpp)
        log(output_cpp, cpp_f)
        print("=== Executing search with py ===")
        output_py = get_output(cmd_py)
        log(output_py, py_f)

        cpp_stats = scrape_search_log(cpp_f)
        py_stats = scrape_search_log(py_f)

        err_msg = "\n"
        for key in ["cost", "expanded", "evaluated", "ratio_seen_colours"]:
            cpp_val = cpp_stats[key]
            py_val = py_stats[key]
            err_msg += f"cpp_{key}: {cpp_val}, py_{key}: {py_val}\n"
        for key in ["cost", "expanded", "evaluated", "ratio_seen_colours"]:
            cpp_val = cpp_stats[key]
            py_val = py_stats[key]
            self.assertTrue(cpp_val == py_val, err_msg)
        print("Good! cpp and python calls match:")
        for key in ["cost", "expanded", "evaluated", "ratio_seen_colours"]:
            cpp_val = cpp_stats[key]
            print(f"{key}: {cpp_val}")


"""
blocksworld-wlf_ilg_gpr
cost: 130
expanded: 181
evaluated: 2282
ratio_seen_colours: 0.8973913231899018

spanner-wlf_ilg_gpr
cost: 61
expanded: 61
evaluated: 3129
ratio_seen_colours: 0.6864474232655486
"""

if __name__ == "__main__":
    if not in_venv():
        print("Not in virtual environment. Exiting...")
        exit(1)
    unittest.main()
