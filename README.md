# goose

## Required packages
See `requirements.txt` if you do not want to use singularity.

## Constructing training dataset
Requires access to `plan_objects.zip`. Do the following steps:
- enter the ```learner``` directory
- create ```data``` directory in the ```learner``` directory
- unzip ```plan_objects.zip``` and put into ```data``` (there should now be a directory ```path_to_goose/learner/data/plan_objects```)
- run the following while in the  ```learner``` directory:
```
python3 scripts/generate_graphs.py llg
```

## Domain-dependent training
To train, go into ```learner``` directory (`cd learner`). Then run 
```
python3 train.py -m RGNN -r llg -d goose-xxx-only --save-file yyy
```
where you replace ```xxx``` by any domain from ```blocks, ferry, gripper, n-puzzle, sokoban, spanner, visitall, visitsome``` and ```yyy``` is the name of the save file ending in `.dt` for the trained weights of the models which would then be located in ```trained_models/yyy``` after training.

## Search evaluation
We use `downward` or `powerlifted` as the search engine which calls code in the `learner` repository for computing heuristics using `pybind11`. To make things simple, we use singularity to contain all our requirements. This ensures you have singularity installed, see [here](https://github.com/apptainer/singularity). Build the singularity container and both Downward and Powerlifted by running in the root repository
```
sh setup.sh
```

Then to run search go into the `learner` directory and execute the `run.py` script with singularity, for example:
```
cd learner
singularity exec --nv ../gpu.sif python3 run.py ../benchmarks/goose/gripper/domain.pddl ../benchmarks/goose/gripper/test/gripper-n20.pddl -m saved-models/dd_llg_gripper.dt -r llg
```

If you do not want to use/have a GPU, you can remove the `--nv` flag. 

Use `-h` for help with arguments or refer to the description below:
```
python3 run.py <DOMAIN_PDDL> <TASK_PDDL> -m <WEIGHTS_FILE> -r <REPRESENTATION>
```
