df=../benchmarks/goose/gripper/domain.pddl
pf=../benchmarks/goose/gripper/test/gripper-n20.pddl
# m=trained_models/slg_gripper_wl_3.joblib
m=trained_models/llg_gripper_wl_3.joblib

singularity exec ../goose.sif python3 run.py $df $pf kernel -m $m
