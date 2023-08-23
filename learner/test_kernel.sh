df=../benchmarks/goose/gripper/domain.pddl
pf=../benchmarks/goose/gripper/test/gripper-n40.pddl

p=fd
# p=pwl

# m=saved_models/slg_gripper_wl_3.joblib
m=saved_models/llg_gripper_wl_3.joblib

singularity exec ../goose.sif python3 run.py $df $pf kernel -m $m
