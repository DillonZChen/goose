REPS="ldg-el fdg-el sdg-el ddg-el"

H=64
p=10

mkdir -p logs

for rep in ldg-el
do
  sbatch --job-name=dd_tvt_${rep} --output=logs/cluster1_dd_train_val_test_${rep}_all_params.log scripts/cluster1_job_3090 "python3 scripts/train_validate_test_dd.py ${rep} -H ${H} -p ${p}"
done
