df=../benchmarks/goose/gripper/domain.pddl
pf=../benchmarks/goose/gripper/test/gripper-n100.pddl

p=fd
# p=pwl

l=linear-svr
i=1
r=llg

m=trained_models_kernel/${l}_${r}_gripper_wl_${i}.joblib

python3 run.py $df $pf linear-regression-opt -m $m -s gbfs
