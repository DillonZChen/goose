LOG_DIR=logs/train_kernel

mkdir -p $LOG_DIR

for l in 0 1 2 3 4
do
  for k in wl
  do
    for r in llg slg dlg glg 
    do
      for d in gripper spanner visitall visitsome blocks ferry sokoban n-puzzle
      do 
        echo $r $k $l $d
        python3 train_kernel.py -k $k -l $l -r $r -d $d --visualise --cross-validation > $LOG_DIR/${r}_${d}_${k}_${l}.log
      done
    done
  done
done