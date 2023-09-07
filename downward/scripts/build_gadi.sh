module load python3
source /scratch/sv11/dc6693/goose_env/bin/activate
module load gcc
rm -rf builds
python3 build.py
