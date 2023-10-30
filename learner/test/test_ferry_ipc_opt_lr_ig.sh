r=ig

m=linear-svr_${r}_ipc2023-learning-ferry_wl_1_0

# python3 scripts_kernel/generate_graphs_kernel.py $r --regenerate -d ipc2023-learning-ferry
domain_pddl=../benchmarks/ipc2023-learning-benchmarks/ferry/domain.pddl
tasks_dir=../benchmarks/ipc2023-learning-benchmarks/ferry/training/easy
plans_dir=../benchmarks/ipc2023-learning-benchmarks/ferry/training_plans
python3 train_kernel.py $domain_pddl $tasks_dir $plans_dir -k wl -l 1 -r $r -d ipc2023-learning-ferry -m linear-svr -p 0 --save-file $m

df=../benchmarks/ipc2023-learning-benchmarks/ferry/domain.pddl
pf=../benchmarks/ipc2023-learning-benchmarks/ferry/training/easy/p99.pddl
pf=../benchmarks/ipc2023-learning-benchmarks/ferry/testing/medium/p30.pddl
# pf=../benchmarks/ipc2023-learning-benchmarks/ferry/testing/hard/p30.pddl

python3 run.py $df $pf linear-regression-opt -m trained_models_kernel/$m -s gbfs

