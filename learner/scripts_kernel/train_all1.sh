LOG_DIR=logs/train_kernel

mkdir -p $LOG_DIR

k=wl
r=ig

for l in 1
do
  for pp in 0
  do 
    p=$((${l}*${pp}))
    for d in ipc2023-learning-ferry ipc2023-learning-blocksworld ipc2023-learning-childsnack ipc2023-learning-floortile ipc2023-learning-miconic ipc2023-learning-rovers ipc2023-learning-satellite ipc2023-learning-sokoban ipc2023-learning-spanner ipc2023-learning-transport
    do 
      for m in mlp
    #   for m in linear-svr lasso ridge rbf-svr quadratic-svr cubic-svr mlp
      do
        SAVE_FILE=${m}_${r}_${d}_${k}_${l}_${p}
        if [ ! -f "trained_models_kernel/${SAVE_FILE}.joblib" ]; then
          echo python3 train_kernel.py -k $k -l $l -r $r -d $d -m $m -p $p --save-file ${SAVE_FILE} '>' $LOG_DIR/${SAVE_FILE}.log
               python3 train_kernel.py -k $k -l $l -r $r -d $d -m $m -p $p --save-file ${SAVE_FILE}  >  $LOG_DIR/${SAVE_FILE}.log
        fi
      done
    done
  done
done
