import os
import subprocess


DOMAINS = [
    "barman",
    "blocksworld",
    "childsnack",
    "ferry",
    "gripper",
    "logistics",
    "miconic",
    "rovers",
    "satellite",
    "spanner",
    "transport",
    "visitall",
]

for domain in DOMAINS:
    pickle_file = f"{domain}.pkl"
    log_file = f"{domain}.log"
    if os.path.exists(pickle_file):
        continue
    cmd = f"./train.py ../goose-policy-25-experiments/benchmarks/{domain}/ configurations/policy.toml --cache {pickle_file}"
    print("*" * 80)
    print(domain)
    print(cmd)
    print(log_file)
    with open(log_file, "w") as log_file:
        subprocess.run(cmd, shell=True, check=True, stdout=log_file, stderr=subprocess.STDOUT)
