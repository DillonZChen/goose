REPS="ldg-el fdg-el sdg-el"

L=4
H=64
aggr=max
p=20

mkdir -p logs

for rep in ldg-el ddg-el
do
  sbatch --job-name=di_tvt_${rep} --output=logs/cluster1_di_train_val_test_${rep}_L${L}_H${H}_${aggr}_p${p}.log scripts/cluster1_job_3090 "python3 scripts/train_validate_test_di.py ${rep} -L ${L} -H ${H} -a ${aggr} -p ${p}"
done
