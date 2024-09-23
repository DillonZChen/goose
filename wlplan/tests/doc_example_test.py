import pathlib

import nbformat
from nbconvert.preprocessors import ExecutePreprocessor

path_to_dir = pathlib.Path(__file__).parent.absolute()


def test_notebook_exec():
    """Only tests that the notebook can be executed with no errors."""

    notebook = f"{path_to_dir}/../docs/examples/blocksworld.ipynb"
    run_path = f"{path_to_dir}/../docs/examples"
    
    with open(notebook) as f:
        nb = nbformat.read(f, as_version=4)
        ep = ExecutePreprocessor(timeout=600, kernel_name="python3")
        try:
            resources = {"metadata": {"path": run_path}}
            assert_msg = f"Got empty notebook for {notebook}"
            assert ep.preprocess(nb, resources=resources) is not None, assert_msg
        except Exception:
            assert False, f"Failed executing {notebook}"


if __name__ == "__main__":
    test_notebook_exec()
