REPS="ldg-el fdg-el sdg-el"

L=8
H=64
aggr=max
p=10

mkdir -p logs

for rep in ldg-el fdg-el sdg-el
do
  sbatch --job-name=dd_tvt_${rep} --output=logs/cluster1_dd_train_val_test_${rep}_L${L}_H${H}_${aggr}_p${p}.log scripts/cluster1_job_3090 "python3 scripts/train_validate_test_dd.py ${rep} -L ${L} -H ${H} -a ${aggr} -p ${p}"
done
