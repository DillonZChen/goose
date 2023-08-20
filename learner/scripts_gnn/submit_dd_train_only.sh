REPS="ldg-el fdg-el sdg-el"

H=64
p=20

mkdir -p logs

for L in 4 8 12 16
do
  for aggr in mean max
  do
    for rep in ldg-el fdg-el sdg-el
    do
      sbatch --job-name=dd_t_${rep} --output=logs/cluster1_dd_train_${rep}_L${L}_H${H}_${aggr}_p${p}.log scripts/cluster1_job_planopt "python3 scripts/train_validate_test_dd.py ${rep} -L ${L} -H ${H} -a ${aggr} -p ${p} --train-only"
    done
  done
done
