# goose

## Required packages
See `requirements.txt`. (Apologies was a bit rushed so not the nicest requirements file). Main packages you need are `torch`, `torch_geometric` and `networkx`. The latest versions should be fine.

## Constructing training dataset
Requires access to `plan_objects.zip`. Do the following steps:
- enter the ```learner``` directory
- create ```data``` directory in the ```learner``` directory
- unzip ```plan_objects.zip``` and put into ```data``` (there should now be a directory ```path_to_goose/learner/data/plan_objects```)
- run the following while in the  ```learner``` directory:
```
python3 scripts/generate_graphs.py sdg-el --regenerate
```

## Domain-dependent training
To train, go into ```learner``` directory (`cd learner`). Then run 
```
python3 train.py -m RGNN -r sdg-el -d goose-xxx-only --save-file yyy
```
where you replace ```xxx``` by any domain from ```blocks, ferry, gripper, n-puzzle, sokoban, spanner, visitall, visitsome``` and ```yyy``` is the name of the save file ending in `.dt` for the trained weights of the models which would then be located in ```trained_models/yyy``` after training.

## Search evaluation
We use `downward` or `powerlifted` as the search engine which calls code in the `learner` repository for computing heuristics using `pybind11`.

For `sdg-el` and `fdg-el` representations (after `-r` flag in training), we will use `downward` for search. For the `ldg-el` representation, we use `powerlifted`. In either case, first set the environment variable
```
export GOOSE=<path-to-goose>/learner
```

To run with a learned model on powerlifted, make sure it is built in the `powerlifted` repository using either `--cpu` or `--gpu` flag depending on whether you want a CPU or GPU build and then execute in the same repository 
```
./powerlifted --xxx -d DOMAIN_FILE -i PROBLEM_FILE -m MODEL_FILE -e gnn -s gbbfs --time-limit TIMEOUT
```
where `--xxx` is either `--cpu` or `--gpu`.


We use a hack using an auxiliary file to get GOOSE to run with `downward`. To run with a learned model on downward, make sure it is built in the `downward` repository and execute in the same repository
```
./fast-downward.py --search-time-limit TIMEOUT DOMAIN_FILE PROBLEM_FILE --search "batch_eager_greedy([goose(graph=CONFIG_FILE)])"
```
where `CONFIG` is `0` if you are using `sdg-el`, and `1` if you are using `fdg-el`. Furthermore, you should have a file `CONFIG_FILE` named `slg` if you are using `sdg-el` and `flg` if you are using `fdg-el` which should contain 3 lines of the form
```
MODEL_FILE
DOMAIN_FILE
PROBLEM_FILE
```
See `fd_cmd` in the file `learner/util/search.py` for more information about this.