# config=blind
config=hff

mkdir -p logs
scp -r gadi:~/downward/logs/ipc2023_${config}.zip logs/
cd logs/
unzip ipc2023_${config}.zip
