MODEL_DIR=icaps24_wl_logs_and_models/models
TRAIN_LOG_DIR=icaps24_wl_logs_and_models/train_logs
TEST_LOG_DIR=icaps24_wl_logs_and_models/test_logs

mkdir -p $MODEL_DIR $TRAIN_LOG_DIR $TEST_LOG_DIR

# for config in svr-linear svr-rbf lwl gpr; do
for config in lwl; do
for domain in blocksworld childsnack ferry floortile miconic rovers satellite sokoban spanner transport; do
for seed in 0 1 2 3 4; do
    desc=${domain}_${config}_r${seed}
    model_file=$MODEL_DIR/$desc.model
    train_log_file=$TRAIN_LOG_DIR/$desc.log
    date
    echo "Training $desc; log at $train_log_file";
    python3 train.py experiments/models/wl_ilg_$config.toml experiments/ipc23-learning/$domain.toml --seed $seed --save-file $model_file > $train_log_file 2>&1;
done;
done;
done;
