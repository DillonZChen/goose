SLURM_SCRIPT=slurm/cluster1_job_gpusrv5_a6000

mkdir -p aaai24_logs
mkdir -p aaai24_logs/slurm

for rep in dlg slg llg # flg
do
    log_file=aaai24_logs/slurm/cluster1_dd_tvt_${rep}.log
    rm -f $log_file

    sbatch --job-name=${rep}_tvt_dd --output=$log_file $SLURM_SCRIPT "python3 slurm/train_validate_test_dd.py -r $rep"
done
