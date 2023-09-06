df=../benchmarks/goose/gripper/domain.pddl
pf=../benchmarks/goose/gripper/test/gripper-n40.pddl

p=fd
# p=pwl

l=linear-svr
i=3
r=llg

m=trained_models_kernel/${l}_${r}_gripper_wl_${i}.joblib

python3 run.py $df $pf kernel-opt -m $m -s gbfs
