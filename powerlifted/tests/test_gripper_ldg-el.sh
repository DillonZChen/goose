DOMAIN="../benchmarks/goose/gripper/domain.pddl"
INSTANCE="../benchmarks/goose/gripper/val/gripper-n11.pddl"
MODEL_PATH="tests/test_gripper_ldg-el.dt"

export GOOSE="$HOME/code/goose/learner"

./powerlifted.py -d $DOMAIN -i $INSTANCE -m $MODEL_PATH -s gbbfs -e gnn --plan-file tests/llg_plan.plan
