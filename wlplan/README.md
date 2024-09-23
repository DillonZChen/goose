WLPlan
======

[![PyPI version](https://badge.fury.io/py/wlplan.svg)](https://pypi.org/project/wlplan/)
[![License](https://img.shields.io/pypi/l/wlplan)](LICENSE)

WLPlan is a package for generating embeddings of PDDL planning problems for machine learning tasks.

## Installation
### Python Interface
The Python interface can be installed simply with

    pip install wlplan

The PyPI release only supports `python>=3.10`.

### C++ Interface
The C++ interface can be installed in your project by running

    ./cmake_build.py <path/to/installation>

and adding the following to the root CMakeLists.txt file of your project

    list(APPEND CMAKE_PREFIX_PATH "<path/to/installation>")
    find_package(wlplan)
    ...
    target_link_libraries(<your_project> PRIVATE wlplan)

## References
For information about the technical details of the underlying algorithm, read the paper [here](https://arxiv.org/abs/2403.16508). The corresponding bib entry is

    @inproceedings{chen-trevizan-thiebaux-icaps2024,
        author       = {Dillon Z. Chen and
                        Felipe W. Trevizan and 
                        Sylvie Thi{\'{e}}baux},
        title        = {Return to Tradition: Learning Reliable Heuristics with Classical Machine Learning},
        booktitle    = {ICAPS},
        year         = {2024},
        pages        = {68--76},
        doi          = {10.1609/ICAPS.V34I1.31462},
    }
