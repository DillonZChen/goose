REPS="ldg-el fdg-el sdg-el gdg-el"

L=16
H=64
p=10

for rep in ldg-el fdg-el sdg-el gdg-el
do
  sbatch --job-name=${rep} --output=logs/cluster1_train_val_${rep}_L${L}_H${H}_p${p}.log scripts/cluster1_job_3090 "python3 scripts/train_validate_dd.py ${rep} -L ${L} -H ${H} -p ${p}"
done

# for rep in ldg-el
# do
#   sbatch --job-name=${rep} --output=logs/cluster1_train_val_${rep}_L${L}_H${H}_p${p}.log scripts/cluster1_job_3090 "python3 scripts/train_validate_dd.py ${rep} -L ${L} -H ${H} -p ${p}"
# done

# for rep in fdg-el
# do
#   sbatch --job-name=${rep} --output=logs/cluster1_train_val_${rep}_L${L}_H${H}_p${p}.log scripts/cluster1_job_3090 "python3 scripts/train_validate_dd.py ${rep} -L ${L} -H ${H} -p ${p}"
# done

# for rep in sdg-el
# do
#   sbatch --job-name=${rep} --output=logs/cluster1_train_val_${rep}_L${L}_H${H}_p${p}.log scripts/cluster1_job_3090 "python3 scripts/train_validate_dd.py ${rep} -L ${L} -H ${H} -p ${p}"
# done

# for rep in gdg-el
# do
#   sbatch --job-name=${rep} --output=logs/cluster1_train_val_${rep}_L${L}_H${H}_p${p}.log scripts/cluster1_job_3090 "python3 scripts/train_validate_dd.py ${rep} -L ${L} -H ${H} -p ${p}"
# done
