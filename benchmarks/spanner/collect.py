import os

domain=os.path.basename(os.getcwd())

os.system(f'rm -rf numeric/training_plans numeric/plan_generation_logs')

from_dir=f'gadi:/scratch/xb83/dc6693/generate_plans/{domain}'

os.system(f'rsync -avz {from_dir}/plans/ numeric/training_plans')
os.system(f'rsync -avz {from_dir}/logs/ numeric/plan_generation_logs')
