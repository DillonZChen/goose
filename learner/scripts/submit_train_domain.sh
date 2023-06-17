DOMAINS="blocks ferry gripper n-puzzle sokoban spanner visitall visitsome"

for domain in blocks ferry gripper n-puzzle sokoban spanner
do
  sbatch --job-name=${domain} --output=logs/cluster1_train_${domain}.log scripts/cluster1_job_any "python3 scripts/train_domain.py ${domain}"
done

for domain in visitall visitsome
do
  sbatch --job-name=${domain} --output=logs/cluster1_train_${domain}.log scripts/cluster1_job_planopt "python3 scripts/train_domain.py ${domain}"
done