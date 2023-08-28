LOG_DIR=logs/train_kernel

mkdir -p $LOG_DIR

for l in 5
# for l in 0 1 2 3 4 5
do
  for k in wl
  do
    for r in slg
    do
      for d in n-puzzle
      do 
        echo $r $k $l $d
        python3 train_kernel.py -k $k -l $l -r $r -d $d --save-file ${r}_${d}_${k}_${l} > $LOG_DIR/${r}_${d}_${k}_${l}.log
      done
    done
  done
done