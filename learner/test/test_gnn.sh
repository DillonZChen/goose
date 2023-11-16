df=../benchmarks/goose/gripper/domain.pddl
pf=../benchmarks/goose/gripper/test/gripper-n40.pddl

p=fd
# p=pwl

m=saved_models/dd_llg_gripper.dt

python3 run.py $df $pf gnn -p $p -m $m
