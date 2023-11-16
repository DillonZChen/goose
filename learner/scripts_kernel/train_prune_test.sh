LOG_DIR=logs/train_kernel

mkdir -p $LOG_DIR

k=wl
r=llg

d=ipc2023-learning-blocksworld
m=linear-svr

for l in 1 3 5
do
  for pp in 0 1 2 3 4 5
  do 
    p=$((${l}*${pp}))
    echo python3 train_kernel.py -k $k -l $l -r $r -d $d -m $m -p $p --save-file ${m}_${r}_${d}_${k}_${l}_${p} '>' $LOG_DIR/${m}_${r}_${d}_${k}_${l}_${p}.log
    python3 train_kernel.py -k $k -l $l -r $r -d $d -m $m -p $p --save-file ${m}_${r}_${d}_${k}_${l}_${p} > $LOG_DIR/${m}_${r}_${d}_${k}_${l}_${p}.log
  done
done
