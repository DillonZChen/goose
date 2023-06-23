REPS="ldg-el fdg-el sdg-el"

L=8
H=64
p=10

# for rep in ldg-el fdg-el sdg-el
# do
#   sbatch --job-name=${rep} --output=logs/cluster1_test_${rep}_L${L}_H${H}_p${p}.log scripts/cluster1_job_3090 "python3 scripts/test_dd.py ${rep} -L ${L} -H ${H} -p ${p}"
# done

for rep in ldg-el
do
  sbatch --job-name=${rep} --output=logs/cluster1_test_${rep}_L${L}_H${H}_p${p}.log scripts/cluster1_job_3090 "python3 scripts/test_dd.py ${rep} -L ${L} -H ${H} -p ${p}"
done

# for rep in fdg-el
# do
#   sbatch --job-name=${rep} --output=logs/cluster1_test_${rep}_L${L}_H${H}_p${p}.log scripts/cluster1_job_3090 "python3 scripts/test_dd.py ${rep} -L ${L} -H ${H} -p ${p}"
# done

# for rep in sdg-el
# do
#   sbatch --job-name=${rep} --output=logs/cluster1_test_${rep}_L${L}_H${H}_p${p}.log scripts/cluster1_job_3090 "python3 scripts/test_dd.py ${rep} -L ${L} -H ${H} -p ${p}"
# done
