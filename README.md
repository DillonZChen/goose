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

