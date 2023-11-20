DOMAIN="../../benchmarks/goose/gripper/domain.pddl"
INSTANCE="../../benchmarks/goose/gripper/test/gripper-n30.pddl"
CONFIG="1"

# 0: slg, 1: flg, 2: llg, 3: glg

export GOOSE="$HOME/code/goose/learner"

cd test

# by default, reopen_closed=false
./../fast-downward.py $DOMAIN $INSTANCE --search "batch_eager_greedy([goose(graph=$CONFIG)])"
