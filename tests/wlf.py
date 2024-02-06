import unittest
from util import *


class TestWlf(unittest.TestCase):
    def test_wlf_ilg_gpr(self):
        # train
        print("========= Training =========")
        cmd = "python3 train.py experiments/models/wlf_ilg_gpr.toml experiments/ipc23-learning/blocksworld.toml --save-file tests/wlf_test.model"
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

        f = "tests/train_wlf.log"
        log((log_output, err_output), f)

        if (
            mse_train is None
            or mse_val is None
            or f1_train is None
            or f1_val is None
        ):
            self.assertTrue(False, "\n".join(err_output))

        self.assertTrue(model_saved)

        self.assertTrue(mse_train < TOL, f"mse_train: {mse_train}")
        self.assertTrue(mse_val < 1.0, f"mse_val: {mse_val}")
        self.assertTrue(f1_train > 1 - TOL, f"f1_train: {f1_train}")
        self.assertTrue(f1_val > 0.5, f"f1_val: {f1_val}")
        print(f"mse_train: {mse_train}")
        print(f"mse_val: {mse_val}")
        print(f"f1_train: {f1_train}")
        print(f"f1_val: {f1_val}")

        # run
        cmd_cpp = "python3 run_wlf.py benchmarks/ipc23-learning/blocksworld/domain.pddl benchmarks/ipc23-learning/blocksworld/testing/medium/p01.pddl tests/wlf_test.model"
        cmd_py = cmd_cpp + " --pybind"

        cpp_f = "tests/run_wlf_cpp.log"
        py_f = "tests/run_wlf_py.log"

        print("========= Executing search with cpp =========")
        output_cpp = get_output(cmd_cpp)
        log(output_cpp, cpp_f)
        print("========= Executing search with py =========")
        output_py = get_output(cmd_py)
        log(output_py, py_f)

        cpp_stats = scrape_search_log(cpp_f)
        py_stats = scrape_search_log(py_f)

        for key in ["cost", "expanded", "evaluated", "ratio_seen_colours"]:
            cpp_val = cpp_stats[key]
            py_val = py_stats[key]
            self.assertTrue(
                cpp_val == py_val, f"cpp_{key}: {cpp_val}, py_{key}: {py_val}"
            )
            print(f"{key}: {cpp_val}")


if __name__ == "__main__":
    if not in_venv():
        print("Not in virtual environment. Exiting...")
        exit(1)
    unittest.main()
