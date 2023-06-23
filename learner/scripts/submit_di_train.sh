REPS="ldg-el fdg-el sdg-el"

L=8
H=64
aggr=mean
p=20

mkdir -p logs

for rep in ldg-el fdg-el sdg-el
do
  sbatch --job-name=di_t_${rep} --output=logs/cluster1_di_train_${rep}_L${L}_H${H}_${aggr}_p${p}.log scripts/cluster1_job_3090 "python3 scripts/train_di.py ${rep} -L ${L} -H ${H} -a ${aggr} -p ${p}"
done
