domain=ferry
difficulty=medium
problem=30

df=../benchmarks/ipc2023-learning-benchmarks/${domain}/domain.pddl
pf=../benchmarks/ipc2023-learning-benchmarks/${domain}/testing/${difficulty}/p${problem}.pddl

p=fd

i=1
r=ig
m=linear-svr
m=mlp

model=trained_models_kernel/${m}_${r}_ipc2023-learning-${domain}_wl_${i}_0.joblib

echo python3 run.py $df $pf kernel-opt -m $model -s gbfs
python3 run.py $df $pf kernel-opt -m $model -s gbfs
