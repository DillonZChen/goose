df=../benchmarks/goose/gripper/domain.pddl
pf=../benchmarks/goose/gripper/test/gripper-n100.pddl

p=fd
# p=pwl

i=3
r=llg
m=linear-svr

model=trained_models_kernel/${m}_${r}_gripper_wl_${i}.joblib

python3 run.py $df $pf kernel-opt -m $model -s gbfs
