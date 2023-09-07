d=miconic

df=../benchmarks/ipc2023-learning-benchmarks/${d}/domain.pddl
pf=../benchmarks/ipc2023-learning-benchmarks/${d}/testing/easy/p30.pddl

p=fd
# p=pwl

l=rbf-svr
i=1
r=llg

m=trained_models_kernel/${l}_${r}_ipc2023-learning-${d}_wl_${i}.joblib

python3 run.py $df $pf kernel-opt -m $m -s gbfs
