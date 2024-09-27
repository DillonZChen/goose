import os
from itertools import product

CUR_DIR = os.path.dirname(os.path.abspath(__file__))
DOMAINS = [
    "blocksworld",
    "childsnack",
    "ferry",
    "miconic",
    "rovers",
    "satellite",
    "spanner",
    "transport",
]
VERSIONS = [
    "classic",
    "numeric",
]

for domain, version in product(DOMAINS, VERSIONS):
    file = f"{CUR_DIR}/{domain}-{version}.toml"
    with open(file, "w") as f:
        f.write(f"[training]\n")
        f.write(f"domain_pddl = 'benchmarks/{domain}/{version}/domain.pddl'\n")
        f.write(f"tasks_dir = 'benchmarks/{domain}/{version}/training'\n")
        f.write(f"plans_dir = 'benchmarks/{domain}/{version}/training_plans'\n")
        f.write(f"numeric = {str(version == 'numeric').lower()}\n")
