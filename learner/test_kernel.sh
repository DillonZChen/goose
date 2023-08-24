df=../benchmarks/goose/gripper/domain.pddl
pf=../benchmarks/goose/gripper/test/gripper-n40.pddl

p=fd
# p=pwl

# m=trained_models_kernel/llg_gripper_wl_1.joblib
m=trained_models_kernel/slg_gripper_wl_1.joblib

singularity exec ../goose.sif python3 run.py $df $pf kernel -m $m
