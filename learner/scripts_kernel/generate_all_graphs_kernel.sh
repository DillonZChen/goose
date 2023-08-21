for rep in llg slg dlg glg flg
do
  echo "python3 dataset/generate_graphs_kernel.py $rep --regenerate"
  python3 dataset/generate_graphs_kernel.py $rep --regenerate
done
