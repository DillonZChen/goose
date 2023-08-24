while true
do
  num_queued=$(qstat | grep "Q " | wc -l)
  echo "$(date) Q=${num_queued} R=$(qstat | grep "R " | wc -l)" | tee -a job_info.log
  to_submit=$((1000 - ${num_queued}))
  echo $to_submit
  python3 scripts_pbs/submit_jobs_kernel.py -e $to_submit
  sleep 1800
done