df=../benchmarks/goose/gripper/domain.pddl
pf=../benchmarks/goose/gripper/test/gripper-n30.pddl

p=fd
# p=pwl

i=3
r=llg

m=trained_models_kernel/${r}_gripper_wl_${i}.joblib

singularity exec ../goose.sif python3 run.py $df $pf kernel -m $m
