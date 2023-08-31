k=wl
d=goose-gripper
r=llg

for m in linear-svr lasso ridge linear-regression
do
  for l in 3
  do
    echo python3 train_kernel.py -k $k -l $l -r $r -d $d -m $m --save-file ${m}_${r}_${d}_${k}_${l}
    python3 train_kernel.py -k $k -l $l -r $r -d $d -m $m --save-file ${m}_${r}_${d}_${k}_${l}
  done
done
