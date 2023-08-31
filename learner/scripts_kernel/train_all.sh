LOG_DIR=logs/train_kernel

mkdir -p $LOG_DIR

k=wl
r=llg


for l in 1 3 5
do
  for d in ipc2023-learning-blocksworld ipc2023-learning-childsnack ipc2023-learning-ferry ipc2023-learning-floortile ipc2023-learning-miconic ipc2023-learning-rovers ipc2023-learning-satellite ipc2023-learning-sokoban ipc2023-learning-spanner ipc2023-learning-transport
  do 
    for m in linear-svr lasso ridge linear-regression
    do
      echo python3 train_kernel.py -k $k -l $l -r $r -d $d -m $m --save-file ${m}_${r}_${d}_${k}_${l}
      python3 train_kernel.py -k $k -l $l -r $r -d $d -m $m --save-file ${m}_${r}_${d}_${k}_${l} > $LOG_DIR/${m}_${r}_${d}_${k}_${l}.log
    done
  done
done
