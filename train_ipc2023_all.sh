for config in svr-linear svr-rbf lwl gpr; do
for domain in blocksworld childsnack ferry floortile miconic rovers satellite sokoban spanner transport; do
    desc=${domain}_${config}
    date
    echo "Training $desc";
    python3 train.py experiments/models/wl_ilg_$config.toml experiments/ipc23-learning/$domain.toml --save-file $desc.model > train_$desc.log 2>&1;
done;
done;
