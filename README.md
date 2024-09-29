# <span style="font-weight:normal">**GOOSE**: **G**raphs **O**ptimised f**O**r **S**earch **E**valuation</span>

GOOSE is a learning for planning framework. It contains various methods for
learning representations for planning tasks, and algorithms for using such
representations for solving planning tasks.

- If you just want to use the WL features, take a look at the
  [WLPlan](https://github.com/DillonZChen/wlplan) package. The
  [dev](https://github.com/DillonZChen/goose/tree/dev) branch illustrates how to
  integrate it into both learning and planning code.
- If you want the code used for our NeurIPS-24 paper, refer to the [NeurIPS-24
  release](https://github.com/DillonZChen/goose/releases/tag/neurips-24).
- If you want the code used for our ICAPS-24 paper, refer to the [ICAPS-24
  release](https://github.com/DillonZChen/goose/releases/tag/icaps-24).
- If you want the code used for our AAAI-24 paper, refer to the [AAAI-24
  release](https://github.com/DillonZChen/goose/releases/tag/aaai-24).
- Future releases will deprecate support for GNN models and will be built around
  the WLPlan package.

See [references](#references) for the corresponding publications.

## Table of contents
- [**GOOSE**: **G**raphs **O**ptimised f**O**r **S**earch **E**valuation](#goose-graphs-optimised-for-search-evaluation)
  - [Table of contents](#table-of-contents)
  - [Setup](#setup)
  - [Training](#training)
    - [Example for WL models](#example-for-wl-models)
    - [Example for GNN models](#example-for-gnn-models)
  - [Planning](#planning)
    - [Example for WL models](#example-for-wl-models-1)
    - [Example for GNN models](#example-for-gnn-models-1)
  - [References](#references)

## Setup
Use the commands below to make a virtual environment, activate it, install
packages, and build cpp components. The setup has been tested with python
versions 3.10 and higher, but should probably work for lower python3 versions as
well.
```
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
sh setup.sh
```

In case a virtual environment does not work, you can also try anaconda:
```
conda create --name goose python=3.10.4
conda activate goose
pip install -r requirements.txt
sh setup.sh
```

## Training
- see `python3 train.py -h` for help, you will need the `--save-file` argument
  if you want to save the model
- to train with your own dataset, you will need to construct an experiment
  configuration toml file such as in
  [here](experiments/ipc23-learning/blocksworld.toml)
  - the `tasks_dir` and `plans_dir` paths must contain the same files,
    differentiating only in the file suffix (.pddl and .plan, respectively)

### Example for WL models
```
python3 train.py experiments/models/wl_ilg_gpr.toml experiments/ipc23-learning/blocksworld.toml --save-file blocksworld_wl.model
```

### Example for GNN models
```
python3 train.py experiments/models/gnn_mean_ilg.toml experiments/ipc23-learning/blocksworld.toml --save-file blocksworld_gnn.model
```

## Planning
- see `run_wl.py` for WL models and `run_gnn.py` for GNN models
- GNN models automatically try to use GPU where possible and CPU otherwise

### Example for WL models
```
python3 run_wl.py benchmarks/ipc23-learning/blocksworld/domain.pddl benchmarks/ipc23-learning/blocksworld/testing/medium/p01.pddl blocksworld_wl.model
```

### Example for GNN models
```
python3 run_gnn.py benchmarks/ipc23-learning/blocksworld/domain.pddl benchmarks/ipc23-learning/blocksworld/testing/medium/p01.pddl blocksworld_gnn.model
```

## References
The relevant publications for this repository are:

- Dillon Ze Chen and Sylvie Thiébaux. **Learning Heuristics for Numeric
  Planning**. NeurIPS 2024. [coming soon]
- Dillon Ze Chen and Felipe Trevizan and Sylvie Thiébaux. **Return to Tradition:
  Learning Reliable Heuristics with Classical Machine Learning**. ICAPS 2024.
  [[pdf](https://dillonzchen.github.io/publications/chen-trevizan-thiebaux-icaps2024.pdf)
  | [bib](https://dblp.org/rec/conf/icaps/ChenTT24.html?view=bibtex)]
- Dillon Ze Chen and Sylvie Thiébaux and Felipe Trevizan. **Learning
  Domain-Independent Heuristics for Grounded and Lifted Planning**. AAAI 2024.
  [[pdf](https://dillonzchen.github.io/publications/chen-thiebaux-trevizan-aaai2024.pdf)
  | [bib](https://dblp.org/rec/conf/aaai/ChenTT24.html?view=bibtex)]
