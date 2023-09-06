# <span style="font-weight:normal">**GOOSE**: **G**raphs **O**ptimised f**O**r **S**earch **E**valuation</span>

## Table of contents
- [**GOOSE**: **G**raphs **O**ptimised f**O**r **S**earch **E**valuation](#goose-graphs-optimised-for-search-evaluation)
  - [Table of contents](#table-of-contents)
  - [Prerequisites](#prerequisites)
  - [GNNs](#gnns)
    - [Search (singularity method)](#search-singularity-method)
    - [Training](#training)
      - [Loading the training dataset](#loading-the-training-dataset)
      - [Domain-dependent training](#domain-dependent-training)
  - [Kernels](#kernels)
    - [Search](#search)
    - [Training](#training-1)

## Prerequisites
You can either use `singularity` or a virtual environment to manage packages. ***TODO*** write this part up

*In either case, you should make sure that you build `downward` or `powerlifted` with your choice of package manager, otherwise you will get runtime errors stating that packages are not found due to using different pybind11 setups.*

## GNNs
### Search (singularity method)
For all commands here, make sure ***not*** to have any python virtual environment activated (e.g. with Anaconda)

We use `downward` or `powerlifted` as the search engine which calls code in the `learner` repository for computing heuristics
using `pybind11`. To make things simple, we use singularity to contain all our requirements. This ensures you have
singularity installed, see [here](https://github.com/apptainer/singularity). Build the singularity container and both
Downward and Powerlifted by running in the root repository
```
sh setup.sh
```

Then to run search go into the `learner` directory and execute the `run_gnn.py` script with singularity, for example:
```
cd learner
singularity exec --nv ../goose.sif python3 run_gnn.py ../benchmarks/goose/gripper/domain.pddl ../benchmarks/goose/gripper/test/gripper-n20.pddl -m saved_models/dd_llg_gripper.dt -r llg
```

The second command can also be called by running `test_gnn.sh`

If you do not want to use/have a GPU, you can remove the `--nv` flag. 

Use `-h` for help with arguments or refer to the description below:
```
python3 run_gnn.py <DOMAIN_PDDL> <TASK_PDDL> -m <WEIGHTS_FILE> -r <REPRESENTATION>
```

### Training
#### Loading the training dataset
Requires access to `plan_objects.zip`. Also requires packages in `requirements.txt` using for example a virtual environment
and `pip install -r requirements.txt`, or alternatively use the singularity container as in [Search](#search). Perform the
following steps
- enter the `learner` directory
- create `data` directory in the `learner` directory
- unzip `plan_objects.zip` and put into `data` (there should now be a directory
  `path_to_goose/learner/data/plan_objects`)
- run the following while in the  `learner` directory:
```
python3 scripts_gnn/generate_graphs_gnn.py --regenerate <REPRESENTATION>
```
for `<REPRESENTATION>` from `llg, dlg, slg, glg, flg` or generate them all at once with
```
sh scripts_gnn/generate_all_graphs_gnn.sh
```

#### Domain-dependent training
Requires packages in `requirements.txt` or alternatively use the singularity container as in [Search](#search). To train, go
into ```learner``` directory (`cd learner`) and run
```
python3 train_gnn.py -m RGNN -r <REPRESENTATION> -d goose-<DOMAIN> --save-file <SAVE_FILE>
```
where you replace `<DOMAIN>` by any domain from `blocks, ferry, gripper, n-puzzle, sokoban, spanner, visitall,
visitsome` and `<SAVE_FILE>` is the name of the save file ending in `.dt` for the trained weights of the models which
would then be located in `trained_models/<SAVE_FILE>` after training.

## Kernels
### Search
TODO
### Training
TODO
