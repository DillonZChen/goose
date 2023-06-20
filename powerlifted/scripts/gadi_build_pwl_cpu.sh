rm -rf builds/cpu_release/

module load singularity
export PLAN_GNN="${HOME}/honours-dillon"

singularity exec /scratch/sv11/dc6693/cpu.sif python3 build.py --cpu
