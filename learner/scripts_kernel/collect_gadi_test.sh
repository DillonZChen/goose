mkdir -p logs/opt_ipc2023_test
ssh gadi "zip -r opt_ipc2023_test.zip goose/learner/logs_kernel/opt_ipc2023_test"
scp gadi:~/opt_ipc2023_test.zip .
unzip opt_ipc2023_test
mv goose/learner/logs_kernel/opt_ipc2023_test/* logs/opt_ipc2023_test
rm -rf goose/learner/logs_kernel/opt_ipc2023_test/
