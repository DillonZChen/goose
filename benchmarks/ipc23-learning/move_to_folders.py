import os

IPC2023_LEARNING_DOMAINS = [  # 90 test instances each
    "blocksworld",
    "childsnack",
    "ferry",
    "floortile",
    "miconic",
    "rovers",
    "satellite",
    "sokoban",
    "spanner",
    "transport",
]

for domain in IPC2023_LEARNING_DOMAINS:
    move_to_dir = f"{domain}/training_plans"
    os.makedirs(move_to_dir, exist_ok=True)
    move_from_dir = f"learning/{domain}"
    for f in os.listdir(move_from_dir):
        if ".plan" not in f:
            continue
        os.system(f"cp {move_from_dir}/{f} {move_to_dir}/{f}")
