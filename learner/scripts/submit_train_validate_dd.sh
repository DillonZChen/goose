DOMAINS="blocks ferry gripper n-puzzle sokoban spanner visitall visitsome"

for domain in blocks ferry gripper n-puzzle sokoban spanner visitall visitsome
do
  sbatch --job-name=${domain} --output=logs/cluster1_train_val_${domain}.log scripts/cluster1_job_3090 "python3 scripts/train_validate_dd.py ${domain}"
done
