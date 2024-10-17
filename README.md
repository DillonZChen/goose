# <span style="font-weight:normal">**GOOSE**: **G**raphs **O**ptimised f**O**r **S**earch **E**valuation</span>

GOOSE is a learning for planning framework. It contains various methods for learning representations for planning tasks, and algorithms for using such representations for solving planning tasks.

If you just want to use the WL features, take a look at the [WLPlan](https://github.com/DillonZChen/wlplan) package.

See [references](#references) for the corresponding publications.

## Table of contents
- [**GOOSE**: **G**raphs **O**ptimised f**O**r **S**earch **E**valuation](#goose-graphs-optimised-for-search-evaluation)
  - [Table of contents](#table-of-contents)
  - [Setup](#setup)
    - [Virtual environment](#virtual-environment)
    - [Apptainer environment](#apptainer-environment)
  - [Training](#training)
  - [Planning](#planning)
  - [Examples](#examples)
  - [References](#references)

## Setup

1. Download submodules with `git submodule update --init --recursive`
2. Set up your environment manually or with a virtual or Apptainer environment described below.
3. Call `sh build.sh` with your environment active

### Virtual environment
Use the commands below to make a virtual environment, activate it, install packages, and build cpp components.
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

### Apptainer environment
We can also use apptainer as an environment. Install apptainer and then build the container

    apt-get install apptainer
    apptainer build goose-environment.sif Environment.def

## Training
Call `python3 train.py -h` for arguments, you will need the `-s` argument if you want to save the model.

## Planning
Call `python3 plan.py -h` for arguments.

## Examples
Training

    python3 train.py configurations/data/ipc23lt/blocksworld.toml configurations/model/gpr_4.toml -s blocks.model

Planning

    python3 plan.py benchmarks/ipc23lt/blocksworld/domain.pddl benchmarks/ipc23lt/blocksworld/testing/p1_01.pddl blocks.model

## References
The GOOSE architecture was initially introduced in our AAAI-24 paper and significantly improved in our ICAPS-24 paper by replacing the deep learning component with a learned linear function. Please refer to the [releases](https://github.com/DillonZChen/goose/releases) page to find the latest version to use or code from a specific publication. The relevant publications so far for this repository are listed as follows.

- Dillon Ze Chen and Felipe Trevizan and Sylvie Thiébaux. **Return to Tradition: Learning Reliable Heuristics with Classical Machine Learning**. ICAPS 2024. [[arxiv](https://arxiv.org/abs/2403.16508) | [bib](https://dblp.org/rec/conf/icaps/ChenTT24.html?view=bibtex)]
- Dillon Ze Chen and Sylvie Thiébaux and Felipe Trevizan. **Learning Domain-Independent Heuristics for Grounded and Lifted Planning**. AAAI 2024. [[arxiv](https://arxiv.org/abs/2312.11143) | [bib](https://dblp.org/rec/conf/aaai/ChenTT24.html?view=bibtex)]
