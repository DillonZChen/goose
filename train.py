import argparse
import os
import sys
import toml
import subprocess


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("model_config")
    parser.add_argument("data_config")
    parser.add_argument("--save-file", default=None)
    args = parser.parse_args()

    model_config = toml.load(args.model_config)
    data_config = toml.load(args.data_config)
    save_file = args.save_file

    train_script = f"train_{model_config['model']['method']}.py"
    data_args = [
        os.path.abspath(data_config["training"][arg])
        for arg in ["domain_pddl", "tasks_dir", "plans_dir"]
    ]
    config_args = []
    for var, val in model_config["config"].items():
        config_args.append(f"--{var}")
        config_args.append(str(val))
    save_args = []
    if save_file is not None:
        save_args = ["--save-file", os.path.abspath(save_file)]
    p = subprocess.Popen(
        ["python3", train_script] + data_args + config_args + save_args,
        cwd="learner",
    )
    sys.exit(p.wait())
