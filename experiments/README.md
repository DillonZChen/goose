To perform the 1000000 sample size hyperparameter study, run the following commands on a slurm cluster.
Note that some sas files have been removed for being too large for GitHub (>100MB)

# 1. Build the `goose.sif` apptainer
In the root directory, run the following commands.
```
git submodule update --init --recursive
sudo apt-get install apptainer
sudo apptainer build goose.sif Apptainer
```

# 2. Train
On a slurm cluster, run the following. You may need to modify the slurm script [train.sh](experiments/train.sh)
```
python3 experiments/submit_train_jobs.py 2000000
```

Logs and models should be automatically placed in `experiments/logs_train` and `experiments/models`

# 3. Plan
On a slurm cluster, run the following. You may need to modify the slurm script [plan.sh](experiments/plan.sh)
```
python3 experiments/submit_plan_jobs.py 2000000
```

Logs should be automatically placed in `experiments/logs_plan`
