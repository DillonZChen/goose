<span style="font-weight:normal">**GOOSE**: **G**raphs **O**ptimised f**O**r **S**earch **E**valuation</span>
=============================================================================================================

GOOSE is a learning for planning framework. It contains various methods for learning representations for planning tasks, and algorithms for using such representations for solving planning tasks. Currently, GOOSE supports [classical grounded](https://github.com/aibasel/downward), [classical lifted](https://github.com/abcorrea/powerlifted), and [numeric planning](https://github.com/Kurorororo/numeric-fast-downward).

If you just want to use the WL features, take a look at the [WLPlan](https://github.com/DillonZChen/wlplan) package.

See [references](#references) for the corresponding publications.

## tl;dr for setup and usage
There are 3 commands to download Goose, train a model, and plan. See further below for more information on how to run Goose for different settings.

```
# (1) Download the Apptainer image
apptainer pull Goose.sif oras://ghcr.io/dillonzchen/goose:latest

# (2) Train 
python3 train.py configurations/data/ipc23lt/blocksworld.toml -s blocksworld.model

# (3) Plan
python3 plan.py benchmarks/ipc23lt/blocksworld/domain.pddl benchmarks/ipc23lt/blocksworld/testing/p1_01.pddl blocksworld.model
```

## Table of contents
- [**GOOSE**: **G**raphs **O**ptimised f**O**r **S**earch **E**valuation](#goose-graphs-optimised-for-search-evaluation)
  - [tl;dr for setup and usage](#tldr-for-setup-and-usage)
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
Install submodules and [Apptainer](https://apptainer.org/) and then build the image

    git submodule update --init --recursive
    sudo apt-get install apptainer
    sudo apptainer build Goose.sif Goose.def


### Manual compilation
You will need the usual cpp packages

    sudo apt-get install build-essential g++ cmake libboost-all-dev

**[Optional]** For numeric planning, you will also need Python2 as we use Numeric Fast Downward which requires it. To install from source (e.g. for Ubuntu 23.04 and above) and symlink to `python2`:

    wget https://www.python.org/ftp/python/2.7.9/Python-2.7.9.tgz
    sudo tar xzf Python-2.7.9.tgz
    cd Python-2.7.9
    sudo ./configure --enable-optimizations
    sudo make altinstall
    sudo ln -s /usr/local/bin/python2.7 /usr/local/bin/python2

Create a virtual environment, activate it, install submodules and packages, and build cpp components.
The setup has been tested with python versions 3.10 and higher, but should probably work for different python3 versions as well.

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
Call `Goose.sif train -h` or `python3 train.py -h` for arguments, you will need the `-s` argument if you want to save the model.
- See below for [recommended training configurations](#recommended-configurations).
- To add your own datasets, follow the directory and `.toml` file structure.
- If you own a CPLEX license and want to train LP models faster, [add it to PYTHONPATH](https://www.ibm.com/docs/en/icos/22.1.1?topic=cplex-setting-up-python-api) and use the manual installation.

e.g.

    # Apptainer
    ./Goose.sif train configurations/data/neurips24/childsnack.toml configurations/model/ccwl/ccwl_rank-lp_1.toml -s numeric_childsnack.model

    # manual installation 
    python3 train.py configurations/data/neurips24/childsnack.toml configurations/model/ccwl/ccwl_rank-lp_1.toml -s numeric_childsnack.model


### Planning
Call `Goose.sif plan -h` or `python3 plan.py -h` for arguments.
e.g.

    # Apptainer
    ./Goose.sif plan benchmarks/neurips24/childsnack/domain.pddl benchmarks/neurips24/childsnack/testing/p2_30.pddl numeric_childsnack.model

    # manual installation
    python3 plan.py benchmarks/neurips24/childsnack/domain.pddl benchmarks/neurips24/childsnack/testing/p2_30.pddl numeric_childsnack.model


### Recommended configurations
For classical planning, train with the default parameters in `options.py`.
e.g. with Blocksworld

    # train
    python3 train.py configurations/data/ipc23lt/blocksworld.toml -s blocksworld.model

    # plan                 
    python3 plan.py benchmarks/ipc23lt/blocksworld/domain.pddl benchmarks/ipc23lt/blocksworld/testing/p1_01.pddl blocksworld.model

For numeric planning, train with the `configurations/model/ccwl/ccwl_rank-lp_1.toml` configuration file.
e.g. with numeric Childsnack

    # train
    python3 train.py configurations/data/neurips24/childsnack.toml configurations/model/ccwl/ccwl_rank-lp_1.toml -s numeric_childsnack.model

    # plan  
    python3 plan.py benchmarks/neurips24/childsnack/domain.pddl benchmarks/neurips24/childsnack/testing/p2_30.pddl numeric_childsnack.model


## References
GOOSE has been published in various venues. Please refer to the [releases](https://github.com/DillonZChen/goose/releases) page to find the latest version to use or code from a specific publication. The relevant publications so far for this repository are listed as follows.

- Dillon Ze Chen and Sylvie Thiébaux. **Graph Learning for Numeric Planning**. NeurIPS 2024.
- Dillon Ze Chen and Felipe Trevizan and Sylvie Thiébaux. **Return to Tradition: Learning Reliable Heuristics with Classical Machine Learning**. ICAPS 2024.
- Dillon Ze Chen and Sylvie Thiébaux and Felipe Trevizan. **Learning Domain-Independent Heuristics for Grounded and Lifted Planning**. AAAI 2024.
