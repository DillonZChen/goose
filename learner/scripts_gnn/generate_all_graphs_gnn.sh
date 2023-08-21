for rep in llg slg dlg glg flg
do
  echo "python3 dataset/generate_graphs_gnn.py $rep --regenerate"
  python3 dataset/generate_graphs_gnn.py $rep --regenerate
done
