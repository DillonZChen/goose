for rep in ldg-el fdg-el sdg-el gdg-el
do
  echo "python3 scripts/generate_graphs.py $rep --regenerate"
  python3 scripts/generate_graphs.py $rep --regenerate
done
