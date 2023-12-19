import os

os.system('rm -rf train')
os.system('rm -rf test')
os.system('rm -rf instances')
os.makedirs('train', exist_ok=True)
os.makedirs('test', exist_ok=True)
os.makedirs('instances', exist_ok=True)

def gen_problem(l,c,s, folder):
    cmd = f"./ferry -l {l} -c {c} -s {s} > {folder}/p-l{l}-c{c}-s{s}.pddl"
    os.system(cmd)
    cmd = f"./ferry -l {l} -c {c} -s {s} > instances/p-l{l}-c{c}-s{s}.pddl"
    os.system(cmd)
    return

for l in range(2, 11):
    for c in range(2, 11):
        for s in range(1, 11):
            gen_problem(l,c,s, 'train')

for i in range(11, 101):
        for s in range(1, 6):
            gen_problem(i,i,s, 'test')
            # gen_problem(11,i,s, 'test')
            # gen_problem(i,11,s, 'test')