domain=blocksworld

rm -rf numeric/training_plans numeric/plan_generation_logs

from=gadi:/scratch/xb83/dc6693/generate_plans/$domain

rsync -avz $from/plans/ numeric/training_plans
rsync -avz $from/logs/ numeric/plan_generation_logs
