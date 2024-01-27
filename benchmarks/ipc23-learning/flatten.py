import os 

for domain in os.listdir("."):
    if not os.path.isdir(domain) or domain == "solutions":
        continue
    # os.system(f"rm -rf {domain}/testing_flattened")
    os.makedirs(f"{domain}/testing_flattened", exist_ok=True)
    for i, diff in enumerate(["easy", "medium", "hard"]):
        for f in os.listdir(f"{domain}/testing/{diff}"):
            prob = f.replace("p", "").replace("ddl", "pddl")
            os.system(f"cp {domain}/testing/{diff}/{f} {domain}/testing_flattened/p{i}_{prob}")
    
