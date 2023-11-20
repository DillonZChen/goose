import os

for i in range(16, 101):
    os.makedirs(f"blocks{i}", exist_ok=True)
    os.system(f"cp bwcgi{i}pddl blocks{i}/slaney_gen.pddl")