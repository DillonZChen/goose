python3 scripts_kernel/generate_graphs_kernel.py lig --regenerate -d goose-gripper

df=../benchmarks/goose/gripper/domain.pddl
pf=../benchmarks/goose/gripper/test/gripper-n100.pddl
# pf=../benchmarks/goose/gripper/train/gripper-n10.pddl

p=fd
# p=pwl

l=linear-svr
i=1
r=lig

mm=${l}_${r}_gripper_wl_${i}
m=trained_models_kernel/${mm}.joblib

# df=../benchmarks/ipc2023-learning-benchmarks/blocksworld/domain.pddl
# pf=../benchmarks/ipc2023-learning-benchmarks/blocksworld/training/easy/p99.pddl
# p=fd
# m=trained_models_kernel/linear-svr_llg_ipc2023-learning-blocksworld_wl_1_0.joblib

python3 train_kernel.py -k wl -l $i -r $r -d goose-gripper -m $l -p 0 --save-file $mm

python3 run.py $df $pf linear-regression-opt -m $m -s gbfs
