REPS="ldg-el fdg-el sdg-el ddg-el"

L=4
H=64
aggr=max

mkdir -p logs

for rep in ddg-el
do
  sbatch --job-name=${rep} --output=logs/cluster1_predict_${rep}_L${L}_H${H}_${aggr}.log scripts/cluster1_job_any "python3 scripts/predict_dd_and_di.py ${rep} -L ${L} -a ${aggr} -H ${H}"
done
