for rep in ldg-el fdg-el sdg-el gdg-el
do
  echo "python3 scripts/generate_graphs.py $rep --regenerate"
  python3 scripts/generate_graphs.py $rep --regenerate
  scp -r data/graphs/$rep/ cluster1:~/goose/learner/data/graphs
done
