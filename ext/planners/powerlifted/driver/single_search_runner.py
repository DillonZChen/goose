import os
import subprocess


def run_single_search(
    build_dir, time, translator_file, search, evaluator, generator, state, seed, plan_file, options, extra
):
    cmd = (
        [
            os.path.join(build_dir, "search", "search"),
            "-f",
            translator_file,
            "-s",
            search,
            "-e",
            evaluator,
            "-g",
            generator,
            "-r",
            state,
            "--seed",
            seed,
            "--domain_path",
            options.domain,
            "--problem_path",
            options.instance,
            "--goose_model_path",
            options.model_path,
        ]
        + ["--plan-file", plan_file]
        + extra
    )
    print(f'Executing "{" ".join(cmd)}"')
    try:
        code = subprocess.call(cmd, timeout=time)
        print("Iteration finished correctly.")
    except subprocess.TimeoutExpired as e:
        print(f"Iteration ran out of time: {e}")
        return -1
    except:
        print(f"Iteration finished with unknown error.")
    return code
