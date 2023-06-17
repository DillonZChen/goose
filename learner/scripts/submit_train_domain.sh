DOMAINS="blocks ferry gripper n-puzzle sokoban spanner visitall visitsome"

for domain in $DOMAINS
do
  sbatch --job-name=${domain} --output=logs/cluster1_train_${domain}.log scripts/cluster1_job_any.sh "python3 scripts/train_domain.py ${domain}"
done