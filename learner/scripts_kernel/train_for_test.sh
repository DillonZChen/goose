l=1
k=wl
d=gripper

for r in slg
do
  echo $r $k $l $d
  python3 train_kernel.py -k $k -l $l -r $r -d $d --save-file ${r}_${d}_${k}_${l}
done