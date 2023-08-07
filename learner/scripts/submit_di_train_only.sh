REPS="ldg-el fdg-el sdg-el"

H=64
p=20

mkdir -p logs

for L in 4
do
  for aggr in max
  do
    for rep in ddg-el
    do
      sbatch --job-name=di_t_${rep} --output=logs/cluster1_di_train_${rep}_L${L}_H${H}_${aggr}_p${p}.log scripts/cluster1_job_planopt "python3 scripts/train_validate_test_di.py ${rep} -L ${L} -H ${H} -a ${aggr} -p ${p} --train-only"
    done
  done
done
