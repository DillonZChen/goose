DOMAIN="../../benchmarks/goose/gripper/domain.pddl"
INSTANCE="../../benchmarks/goose/gripper/test/gripper-n15.pddl"
CONFIG="2"

# 0: slg, 1: flg, 2: llg, 3: glg

export GOOSE="$HOME/code/goose/learner"

cd tests

./../fast-downward.py $DOMAIN $INSTANCE --search "eager_greedy([goose(graph=$CONFIG)])"
