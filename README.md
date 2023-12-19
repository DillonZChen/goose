# <span style="font-weight:normal">**GOOSE**: **G**raphs **O**ptimised f**O**r **S**earch **E**valuation</span>

This branch corresponds to the source code used in our AAAI-24 publication "Learning Domain-Independent Heuristics for Grounded and Lifted Planning"

## Table of contents
- [**GOOSE**: **G**raphs **O**ptimised f**O**r **S**earch **E**valuation](#goose-graphs-optimised-for-search-evaluation)
  - [Table of contents](#table-of-contents)
  - [Setup](#setup)
  - [Training](#training)
  - [Search](#search)
  - [AAAI-24 experiment pipelines](#aaai-24-experiment-pipelines)

## Setup
We will use [Singularity/Apptainer](https://github.com/apptainer/singularity) to manage packages. This is more stable than using a conda or python virtual environment. This is because we will be using pybind11 which allows us to call python code from c++ and sometimes it does not work properly with virtual environments.

First ensure you have Singularity/Apptainer installed.

Then build the containers by executing
```
sudo singularity goose_gpu.sif goose_gpu.def
```
if you plan to run GOOSE on a GPU (requires cuda version 11.8 or higher) and otherwise replace `goose_gpu` with `goose_cpu`.

Then execute any commands below by executing 
```
singularity exec --nv goose_gpu.sif $CMD
```
where `$CMD` corresponds to your command. If do not have a GPU, you can remove `--nv ` and even run `./goose_cpu.sif $CMD`

Nevertheless, you can still try to use just a virtual environment for example by
```
python3 -m venv venv
. venv/bin/activate
pip install -r requirements.txt
```

## Training
The underlying pipeline for a training a model is
- generating states from PDDL and optimal plan files
- converting the states into graphs in tensor format for GNNs
- training the GNN on the graphs

To train a model enter the learner directory with `cd learner` and run the `train_gnn.py` script. For example to train a Blocksworld model, run
```
python3 train_gnn.py blocks -L 8 --aggr mean --rep llg --save-file blocks_llg_mean_8.dt
```
which trains a GNN model operating on the LLG representation of planning tasks with 8 message passing layers and mean aggregation function. The trained model weights are then saved to `blocks_llg_mean_8.dt`

To train a domain-independent model, execute
```
python3 train_gnn.py ipc
```
and add additional arguments as necessary. Use `-h` or `--help` for more details on arguments.

## Search
We use `downward` or `powerlifted` as the search engine which calls code in the `learner` repository for computing heuristics using `pybind11`. 
First make sure you have built Downward and Powerlifted by running in the root repository
```
python3 setup.py gpu
```
or with `cpu` instead of `gpu` if no GPU is available.

Then to run search go into the `learner` directory and execute the `run_gnn.py` script with singularity, for example:
```
cd learner
singularity exec --nv ../goose_gpu.sif python3 run_gnn.py ../benchmarks/goose/blocks/domain.pddl ../benchmarks/goose/blocks/test/blocks25-task01.pddl blocks_llg_mean_8.dt
```

More generally, we have
```
python3 run_gnn.py <domain_pddl> <task_pddl> <model_weights>
```

## AAAI-24 experiment pipelines
In the `learner` directory, execute
```
python3 train_validate_test_dd.py -r <representation>
```
for domain-dependent experiments and 
```
python3 train_validate_test_di.py -r <representation>
```
for domain-independent experiments where `<representation>` is one of `slg`, `flg`, `llg`.
