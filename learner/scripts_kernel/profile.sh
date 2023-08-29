df=../benchmarks/goose/gripper/domain.pddl
pf=../benchmarks/goose/gripper/test/gripper-n40.pddl

p=fd
# p=pwl

i=3
r=llg

m=trained_models_kernel/${r}_gripper_wl_${i}.joblib

export GOOSE=/home/dillon/code/goose/learner

singularity exec ../goose_valgrind.sif valgrind --tool=callgrind --callgrind-out-file=callgrind.out --dump-instr=yes --collect-jumps=yes /home/dillon/code/goose/downward/builds/release/bin/downward --search 'batch_eager_greedy([kernel(model_path="trained_models_kernel/llg_gripper_wl_3.joblib", domain_file="../benchmarks/goose/gripper/domain.pddl", instance_file="../benchmarks/goose/gripper/test/gripper-n40.pddl")])' --internal-plan-file plans/fd_-benchmarks-goose-gripper-test-gripper-n40_batch_eager_greedy_llg_gripper_wl_3joblib.plan < sas_files/fd_-benchmarks-goose-gripper-test-gripper-n40_batch_eager_greedy_llg_gripper_wl_3joblib.sas_file