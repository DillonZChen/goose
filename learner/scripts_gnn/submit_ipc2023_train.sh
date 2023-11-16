mkdir -p logs

for i in 0
do
for domain in blocksworld childsnack ferry floortile miconic rovers satellite sokoban spanner transport
do
  sbatch --job-name=ipc2023_${domain} --output=logs/ipc2023_train_${domain}.log scripts/cluster1_job_any "python3 train_gnn.py -r llg -m RGNN -p 10 -L 4 -H 64 -d ipc2023-learning-${domain} --save-file ipc2023-learning-${domain}-${i}"
done

for domain in spanner transport
do
  sbatch --job-name=ipc2023_${domain} --output=logs/ipc2023_train_${domain}.log scripts/cluster1_job_planopt "python3 train_gnn.py -r llg -m RGNN -p 10 -L 4 -H 64 -d ipc2023-learning-${domain} --save-file ipc2023-learning-${domain}-${i}"
done
done
