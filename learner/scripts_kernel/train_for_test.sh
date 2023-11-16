k=wl
d=goose-gripper
r=llg

for d in goose-gripper goose-spanner
do
  for m in linear-regression linear-svr lasso ridge rbf-svr quadratic cubic
  do
    for l in 1 3
    do
      echo
      echo python3 train_kernel.py -k $k -l $l -r $r -d $d -m $m --save-file ${m}_${r}_${d}_${k}_${l}
      python3 train_kernel.py -k $k -l $l -r $r -d $d -m $m --save-file ${m}_${r}_${d}_${k}_${l}
    done
  done
done
