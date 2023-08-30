k=wl
d=gripper
r=llg

m=linear-svr
for l in 1 3 5
do
  echo python3 train_kernel.py -k $k -l $l -r $r -d $d -m $m --save-file ${m}_${r}_${d}_${k}_${l}
  python3 train_kernel.py -k $k -l $l -r $r -d $d -m $m --save-file ${m}_${r}_${d}_${k}_${l}
done

m=lasso
for l in 1 3 5
do
  echo python3 train_kernel.py -k $k -l $l -r $r -d $d -m $m --save-file ${m}_${r}_${d}_${k}_${l}
  python3 train_kernel.py -k $k -l $l -r $r -d $d -m $m --save-file ${m}_${r}_${d}_${k}_${l}
done

# m=ridge
# for l in 1 3 5
# do
#   echo python3 train_kernel.py -k $k -l $l -r $r -d $d -m $m --save-file ${m}_${r}_${d}_${k}_${l}
#   python3 train_kernel.py -k $k -l $l -r $r -d $d -m $m --save-file ${m}_${r}_${d}_${k}_${l}
# done
