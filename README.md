<span style="font-weight:normal">**GOOSE**: **G**raphs **O**ptimised f**O**r **S**earch **E**valuation</span>
=============================================================================================================

GOOSE is a learning for planning framework. It contains various methods for learning representations for planning tasks, and algorithms for using such representations for solving planning tasks.

If you just want to use the WL features, take a look at the [WLPlan](https://github.com/DillonZChen/wlplan) package.

See [references](#references) for the corresponding publications.

## Table of contents
- [**GOOSE**: **G**raphs **O**ptimised f**O**r **S**earch **E**valuation](#goose-graphs-optimised-for-search-evaluation)
  - [Table of contents](#table-of-contents)
  - [Setup](#setup)
    - [Apptainer image](#apptainer-image)
    - [Manual compilation](#manual-compilation)
  - [Usage](#usage)
    - [Training](#training)
    - [Planning](#planning)
    - [Recommended configurations](#recommended-configurations)
  - [References](#references)

## Setup

### Apptainer image
Install submodules and [Apptainer](https://apptainer.org/) and then build the images

    git submodule update --init --recursive
    sudo apt-get install apptainer
    sudo apptainer build GooseLearner.sif GooseLearner.def
    sudo apptainer build GoosePlanner.sif GoosePlanner.def


### Manual compilation
Create a virtual environment, activate it, install submodules and packages, and build cpp components.
The setup has been tested with python versions 3.10 and higher, but should probably work for lower python3 versions as well.

    python3 -m venv venv
    source venv/bin/activate
    pip install -r requirements.txt
    git submodule update --init --remote --recursive
    sh build.sh

In case a virtual environment does not work, you can also try anaconda and specify a Python version:

    conda create --name goose python=3.10.4
    conda activate goose
    pip install -r requirements.txt
    git submodule update --init --remote --recursive
    sh build.sh


## Usage
### Training
Call `GooseLearner.sif -h` or `python3 train.py -h` for arguments, you will need the `-s` argument if you want to save the model.
See below for [recommended training configurations](#recommended-configurations).
To add your own datasets, follow the directory and `.toml` file structure.

For example with Apptainer:

    ./GooseLearner.sif configurations/data/neurips24/childsnack.toml configurations/model/ccwl/ccwl_rank-lp_1.toml -s numeric_childsnack.model

or with a manual installation:

    python3 train.py configurations/data/neurips24/childsnack.toml configurations/model/ccwl/ccwl_rank-lp_1.toml -s numeric_childsnack.model


### Planning
Call `GoosePlanner.sif -h` or `python3 plan.py -h` for arguments.

For example with Apptainer:

    ./GoosePlanner.sif benchmarks/neurips24/childsnack/domain.pddl benchmarks/neurips24/childsnack/testing/p2_30.pddl numeric_childsnack.model

or with a manual installation:

    python3 plan.py benchmarks/neurips24/childsnack/domain.pddl benchmarks/neurips24/childsnack/testing/p2_30.pddl numeric_childsnack.model


### Recommended configurations
For classical planning, train with the `configurations/model/wl/wl_rank-lp_3.toml` configuration file.

For numeric planning, train with the `configurations/model/ccwl/ccwl_rank-lp_1.toml` configuration file.


## References
GOOSE has been published in various venues. Please refer to the [releases](https://github.com/DillonZChen/goose/releases) page to find the latest version to use or code from a specific publication. The relevant publications so far for this repository are listed as follows.

- Dillon Ze Chen and Sylvie Thiébaux. **Graph Learning for Numeric Planning**. NeurIPS 2024.
- Dillon Ze Chen and Felipe Trevizan and Sylvie Thiébaux. **Return to Tradition: Learning Reliable Heuristics with Classical Machine Learning**. ICAPS 2024.
- Dillon Ze Chen and Sylvie Thiébaux and Felipe Trevizan. **Learning Domain-Independent Heuristics for Grounded and Lifted Planning**. AAAI 2024.
