LOG_DIR=logs/train_kernel

mkdir -p $LOG_DIR

k=wl
r=ig

for l in 1
do
  for pp in 0
  do 
    p=$((${l}*${pp}))
    # for d in ipc2023-learning-ferry ipc2023-learning-blocksworld ipc2023-learning-childsnack ipc2023-learning-floortile ipc2023-learning-miconic ipc2023-learning-rovers ipc2023-learning-satellite ipc2023-learning-sokoban ipc2023-learning-spanner ipc2023-learning-transport
    for d in ferry blocksworld childsnack floortile miconic rovers satellite sokoban spanner transport
    do 
      for m in linear-svr lasso ridge rbf-svr quadratic-svr cubic-svr mlp
      do
        SAVE_FILE=opt_${m}_${r}_${d}_${k}_${l}_${p}
        if [ ! -f "trained_models_kernel/${SAVE_FILE}.joblib" ]; then
            domain_pddl=../benchmarks/ipc2023-learning-benchmarks/$d/domain.pddl
            tasks_dir=../benchmarks/ipc2023-learning-benchmarks/$d/training/easy
            plans_dir=../benchmarks/ipc2023-learning-benchmarks/$d/training_plans

          echo python3 train_kernel.py $domain_pddl $tasks_dir $plans_dir -k $k -l $l -r $r -d ipc2023-learning-$d -m $m -p $p --save-file ${SAVE_FILE} '>' $LOG_DIR/${SAVE_FILE}.log
               python3 train_kernel.py $domain_pddl $tasks_dir $plans_dir -k $k -l $l -r $r -d ipc2023-learning-$d -m $m -p $p --save-file ${SAVE_FILE}  >  $LOG_DIR/${SAVE_FILE}.log
        fi
      done
    done
  done
done
