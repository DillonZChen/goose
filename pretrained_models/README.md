# Pretrained GOOSE models
These are models trained for benchmarks used in our ICAPS'24 and NeurIPS'24 paper. 
The models were trained with [WLPlan](https://github.com/DillonZChen/wlplan) `v1.2.1-pre` and [GOOSE](https://github.com/DillonZChen/goose) `v1.2.2`.

## Classic (IPC23LT Benchmarks)
The configuration used is
```
features = "wl"
graph_representation = "ilg"
iterations = 2
optimisation = "rank-svm"
feature_pruning = "i-mf"
data_pruning = "equivalent-weighted"
data_generation = "plan"
facts = "fd"
hash = "set"
```
with the exception of Blocksworld where `feature_pruning = "none"`. 
With this setting, the coverage on Blocksworld changes from 55 to 85, and the overall coverage from 527 to 557.

### Coverage results
- 1800s search runtime
- 8GB memory
- Intel® Xeon® Processor E5-2695 v3 processes

| Domain      | Solved |
| :---------- | -----: |
| Blocksworld |     85 |
| Childsnack  |     24 |
| Ferry       |     77 |
| Floortile   |      3 |
| Miconic     |     90 |
| Rovers      |     56 |
| Satellite   |     57 |
| Sokoban     |     37 |
| Spanner     |     72 |
| Transport   |     56 |
| Total       |    557 |

## Numeric (NeurIPS'24 Benchmarks)
For all NeurIPS'24 domains
```
features = "ccwl"
graph_representation = "nilg"
iterations = 1
optimisation = "rank-lp"
feature_pruning = "none"
data_pruning = "none"
data_generation = "plan"
facts = "nfd"
hash = "set"
```

### Coverage results
- 300s search runtime
- 8GB memory
- Intel® Xeon® Processor E5-2695 v3 processes

| Domain      | Solved |
| :---------- | -----: |
| Blocksworld |     30 |
| Childsnack  |     90 |
| Ferry       |     69 |
| Miconic     |     88 |
| Rovers      |     21 |
| Satellite   |     27 |
| Spanner     |     90 |
| Transport   |     52 |
| Total       |    467 |
