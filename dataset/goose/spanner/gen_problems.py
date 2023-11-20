import os

os.system('rm -rf train')
os.system('rm -rf test')
os.system('rm -rf instances')
os.makedirs('train', exist_ok=True)
os.makedirs('test', exist_ok=True)
os.makedirs('instances', exist_ok=True)

def gen_problem(s,n, seed, folder):
    l = int((s+n)/3)  # ceiling
    problem_name = f'p-s{s}-n{n}-l{l}-seed{seed}'
    cmd = f"python3 spanner-generator.py --seed {seed} --problem-name {problem_name} {s} {n} {l}"
    os.system(f'{cmd} > {folder}/{problem_name}.pddl')
    os.system(f'{cmd} > instances/{problem_name}.pddl')
    return

for s in range(2, 11):
    for n in range(2, 11):
        for seed in range(1, 11):
            gen_problem(s,n,seed, 'train')

for i in range(11, 101):
        for seed in range(1, 6):
            gen_problem(i,i,seed, 'test')