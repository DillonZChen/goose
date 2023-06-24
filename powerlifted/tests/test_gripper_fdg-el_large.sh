DOMAIN="../benchmarks/goose/gripper/domain.pddl"
INSTANCE="../benchmarks/goose/gripper/test/gripper-n30.pddl"
MODEL_PATH="tests/test_gripper_fdg-el.dt"

export GOOSE="$HOME/code/goose/learner"

./powerlifted.py -d $DOMAIN -i $INSTANCE -m $MODEL_PATH -s gbbfs -e gnn --plan-file tests/flg_plan.plan
