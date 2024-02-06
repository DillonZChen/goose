# <span style="font-weight:normal">**GOOSE**: **G**raphs **O**ptimised f**O**r **S**earch **E**valuation</span>

GOOSE is a learning for planning framework. It contains various methods for learning representations for planning tasks, and algorithms for using such representations for solving planning tasks.

See [references](#references) for the corresponding publications.

## Table of contents
- [**GOOSE**: **G**raphs **O**ptimised f**O**r **S**earch **E**valuation](#goose-graphs-optimised-for-search-evaluation)
  - [Table of contents](#table-of-contents)
  - [Setup](#setup)
  - [Training](#training)
    - [Example for WL models](#example-for-wl-models)
    - [Example for GNN models](#example-for-gnn-models)
  - [Heuristic Search](#heuristic-search)
    - [Example for WL models](#example-for-wl-models-1)
    - [Example for GNN models](#example-for-gnn-models-1)
  - [References](#references)
    - [AAAI-24 Experiments](#aaai-24-experiments)
    - [Bibtex files](#bibtex-files)
  - [Code acknowledgements](#code-acknowledgements)

## Setup
Use the commands below to make a virtual environment, activate it, install packages, and build cpp components.
The setup has been tested with python versions 3.10 and higher, but should probably work for lower python3 versions
```
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
sh build_components.sh
```

## Training
- see `python3 train.py -h` for help, you will need the `--save_file` argument if you want to save the model
- to train with your own dataset, you will need to construct an experiment configuration toml file such as in [here](experiments/ipc23-learning/blocksworld.toml)
  - the `tasks_dir` and `plans_dir` paths must contain the same files, differentiating only in the file suffix (.pddl and .plan, respectively)

### Example for WL models
```
python3 train.py experiments/models/wlf_ilg_gpr.toml experiments/ipc23-learning/blocksworld.toml --save_file blocksworld_wlf.model
```

### Example for GNN models
```
python3 train.py experiments/models/gnn_mean_ilg.toml experiments/ipc23-learning/blocksworld.toml --save_file blocksworld_gnn.model
```

## Heuristic Search
- see `run_wlf.py` for WL models and `run_gnn.py` for GNN models
- GNN models automatically try to use GPU where possible and CPU otherwise

### Example for WL models
```
python3 run_wlf.py benchmarks/ipc23-learning/blocksworld/domain.pddl benchmarks/ipc23-learning/blocksworld/testing/medium/p01.pddl blocksworld_wlf.model
```

### Example for GNN models
```
python3 run_gnn.py benchmarks/ipc23-learning/blocksworld/domain.pddl benchmarks/ipc23-learning/blocksworld/testing/medium/p01.pddl blocksworld_gnn.model
```

## References
The relevant publications for this repository are:

- Dillon Ze Chen and Sylvie Thiébaux and Felipe Trevizan. **Learning Domain-Independent Heuristics for Grounded and Lifted Planning**. AAAI 2024. [[pdf](https://dillonzchen.github.io/publications/Chen2024Goose.pdf)]
- Dillon Ze Chen and Sylvie Thiébaux and Felipe Trevizan. **GOOSE: Learning Domain-Independent Heuristics**. Genplan 2023. [[pdf](https://dillonzchen.github.io/publications/Chen2023Wl.pdf)]
- Dillon Ze Chen and Felipe Trevizan and Sylvie Thiébaux. **Graph Neural Networks and Graph Kernels For Learning Heuristics: Is there a difference?**. Genplan 2023. [[pdf](https://dillonzchen.github.io/publications/Chen2023Goose.pdf)]

### AAAI-24 Experiments
For source code corresponding to experiments from our AAAI-24 publication, please refer to this [release](https://github.com/DillonZChen/goose/releases/tag/v1.0.0).

### Bibtex files
For the bibtex file for GNN architectures using the `slg`, `flg` and `llg` graph representations:
```
@inproceedings{chen-et-al:aaai2024,
  author       = {Dillon Ze Chen and
                  Sylvie Thi{\'{e}}baux and
                  Felipe W. Trevizan},
  title        = {Learning Domain-Independent Heuristics for Grounded and Lifted Planning},
  booktitle    = {AAAI},
  year         = {2024},
}
```

For the bibtex file for WL and GNN architectures using the `ilg` graph representations:
```
@inproceedings{chen-et-al:genplan2023,
  author       = {Dillon Ze Chen and
                  Felipe W. Trevizan and 
                  Sylvie Thi{\'{e}}baux},
  title        = {Graph Neural Networks and Graph Kernels For Learning Heuristics: Is there a difference?},
  booktitle    = {Seventh Workshop on Generalization in Planning (GenPlan)},
  year         = {2023},
}
```

## Code acknowledgements
- [Fast Downward](https://github.com/aibasel/downward) by Malte Helmert and various contributors for the planning parser, grounder and grounded search algorithm.
- [Powerlifted](https://github.com/abcorrea/powerlifted) by Augusto B. Corrêa and various contributors for their lifted planner.
- All the other various packages listed in the requirements
