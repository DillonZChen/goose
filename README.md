# numeric-GOOSE

This branch extends the GOOSE framework to handle numeric planning and contains
code for experiments in our NeurIPS-24 paper. The code here will be replaced and
integrated into the [WLPlan](https://github.com/DillonZChen/wlplan) package some
time in the future.

## Table of contents

- [numeric-GOOSE](#numeric-goose)
  - [Table of contents](#table-of-contents)
  - [Setup](#setup)
    - [Environment with Anaconda](#environment-with-anaconda)
    - [Environment with Apptainer/Singularity \[Alternative\]](#environment-with-apptainersingularity-alternative)
    - [Installation](#installation)
    - [CPLEX solver \[Optional\]](#cplex-solver-optional)
  - [Training](#training)
    - [Creating your own dataset](#creating-your-own-dataset)
  - [Planning](#planning)


## Setup
This code is developed and supported for Linux Ubuntu 22.04. Some setup
procedures below may need to be modified for different operating systems.

### Environment with Anaconda
Install python2 for [Numeric Fast
Downward](https://github.com/Kurorororo/numeric-fast-downward) (NFD) translator,
and packages for compiling NFD:

    sudo apt-get install python2 build-essential cmake


Package management with conda:

    conda create --name ngoose python=3.10.4
    conda activate ngoose
    pip install -r requirements.txt


### Environment with Apptainer/Singularity [Alternative]
We can build a container that we can use as a virtual environment:

    apptainer build ngoose_env_cpu.sif Environment_cpu.def

or 

    apptainer build ngoose_env_gpu.sif Environment_gpu.def

if you want to compare with the neural network models with GPUs. Note that the
GPU container is significantly larger in size than the CPU container due to CUDA
dependencies (e.g. 7.0GB vs 867MB).

To call any other command below:

    $ apptainer exec [--nv] ngoose_env_<DEVICE>.sif <CMD>

where `--nv` is needed if you want to use CUDA

### Installation

    sh setup.sh

Remember to activate your environment beforehand or use a container.

### CPLEX solver [Optional]
CPLEX can be used to solve MIPs faster. To use CPLEX, you will need a license.
It is possible to get a free academic license from IBM by following these steps
and install CPLEX:
  - Go to this [link](https://www.ibm.com/academic/topic/data-science)
  - Login or register for an account
  - Scroll down and select the Software tab, then select the ILOG CPLEX
    Optimization Studio option, and follow the instructions from there

To use CPLEX as the backend MIP solver, set the PYTHONPATH environment variable
with the correct path, e.g.

    export PYTHONPATH=/opt/ibm/ILOG/CPLEX_Studio2211/cplex/python/3.10/x86-64_linux/

Otherwise, the code will use the free COIN CBC solver.

## Training
Call `python3 train.py -h` for help, you will need the `--save_file` argument if
you want to save a model. For example,

    python3 train.py configurations/model/r-wlf1.toml configurations/data/childsnack-numeric.toml --save_file childsnack.model

will train a ranking estimator as a heuristic function for the Childsnack domain
with ccWL features.

Do not forget to set the PYTHONPATH environment variable (as described
[here](#cplex-solver-optional)) if you have CPLEX and are training a ccWL
ranking model.

### Creating your own dataset
Create a `.toml` file of the form

    [training]
    domain_pddl = <PATH_TO_DOMAIN_PDDL_FILE>
    tasks_dir = <PATH_TO_DIRECTORY_OF_PROBLEM_PDDL_FILES>
    plans_dir = <PATH_TO_DIRECTORY_OF_PLAN_FILES>

Every plan file in `plans_dir` must end in `.plan` and must have a corresponding
problem file with the same file name in `tasks_dir` with `.plan` replaced by
`.pddl`.

The code automatically generates successors of states from plan traces for
ranking models.

## Planning
Call `python3 plan.py -h` for help. For example,
    
    python3 plan.py benchmarks/childsnack/numeric/domain.pddl benchmarks/childsnack/numeric/testing/p2_30.pddl childsnack.model

will give you a plan for the hardest Childsnack problem in around 10 seconds
depending on your hardware.
