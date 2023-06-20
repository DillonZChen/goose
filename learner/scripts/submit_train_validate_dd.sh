REPS="ldg-el fdg-el sdg-el gdg-el"

for rep in ldg-el
do
  sbatch --job-name=${rep} --output=logs/cluster1_train_val_${rep}.log scripts/cluster1_job_any "python3 scripts/train_validate_dd.py ${rep}"
done

# for rep in fdg-el
# do
#   sbatch --job-name=${rep} --output=logs/cluster1_train_val_${rep}.log scripts/cluster1_job_any "python3 scripts/train_validate_dd.py ${rep}"
# done

# for rep in sdg-el
# do
#   sbatch --job-name=${rep} --output=logs/cluster1_train_val_${rep}.log scripts/cluster1_job_any "python3 scripts/train_validate_dd.py ${rep}"
# done

# for rep in gdg-el
# do
#   sbatch --job-name=${rep} --output=logs/cluster1_train_val_${rep}.log scripts/cluster1_job_any "python3 scripts/train_validate_dd.py ${rep}"
# done
