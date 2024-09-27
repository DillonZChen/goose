import os

CUR_DIR = os.path.dirname(os.path.abspath(__file__))

print("Running training and planning pipelines. They should all complete in a minute or two.")

for f in sorted(os.listdir(CUR_DIR)):
    if not f.endswith(".sh"):
        continue

    log_file = os.path.join(CUR_DIR, f.replace(".sh", ".log"))

    print("="*80)
    print(f"Running {f}")
    print(f"Logging to {log_file}")
    print("="*80)
    os.system(f"bash {os.path.join(CUR_DIR, f)} >> {log_file} 2>&1")
