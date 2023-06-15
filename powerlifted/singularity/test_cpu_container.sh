export PLAN_GNN="${HOME}/honours/honours-code"

d=hgn-benchmarks/blocks/domain.pddl
i=hgn-benchmarks/blocks/test/blocks12-task01.pddl
s=gbfs
m=trained_models_0_H64/fdr-pdg-blocks_0_0.dt
# m=trained_models_0_H64/fdr-pdg-el-blocks_0_0.dt

singularity exec cpu.sif python3 powerlifted.py --cpu -d $d -i $i -s $s -m $m -e gnn
