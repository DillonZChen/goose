df=../benchmarks/goose/gripper/domain.pddl
pf=../benchmarks/goose/gripper/test/gripper-n100.pddl

p=fd
# p=pwl

i=1
r=ig

for m in mlp
# for m in linear-svr ridge lasso linear-regression
do
  model=trained_models_kernel/${m}_${r}_gripper_wl_${i}_0.joblib

  echo python3 run.py $df $pf kernel-opt -m $model -s gbfs
  python3 run.py $df $pf kernel-opt -m $model -s gbfs > ${m}.log
done
