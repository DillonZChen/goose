SLURM_SCRIPT=slurm/cluster1_job_gpusrv3

rep=llg

mkdir -p aaai24_logs
mkdir -p aaai24_logs/slurm

for domain in gripper spanner visitall visitsome blocks ferry sokoban n-puzzle
do
    log_file=aaai24_logs/slurm/cluster1_dd_tvt_${rep}_${domain}.log
    rm -f $log_file

    sbatch --job-name=${rep}_${domain} --output=$log_file $SLURM_SCRIPT "python3 slurm/train_validate_test_dd.py -r $rep -d $domain"
done
