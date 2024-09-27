r_wlf = """
[model]
method = "wlf"
target = "r"

[data]
val_ratio = 0.0

[representation]
cat_iterations = <LAYERS>
con_iterations = <LAYERS>

[estimator]
estimator = "miprk"
round = true
"""

h_wlf = """
[model]
method = "wlf"
target = "h"

[data]
val_ratio = 0.0

[representation]
cat_iterations = <LAYERS>
con_iterations = <LAYERS>

[estimator]
estimator = "gpr"
round = true
"""

r_gnn = """
[model]
method = "gnn"
target = "r"

[data]
val_ratio = 0.0

[representation]
dynamic_features = false

[estimator]
jumping_knowledge = false
round = true
aggr = "max"
pool = "sum"
nhid = 64
nlayers = <LAYERS>

[opt]
batch_size = 16
lr = 0.001
l2 = 0.001
"""

h_gnn = """
[model]
method = "gnn"
target = "h"

[data]
val_ratio = 0.25

[representation]
dynamic_features = false

[estimator]
jumping_knowledge = false
round = true
aggr = "max"
pool = "sum"
nhid = 64
nlayers = <LAYERS>

[opt]
batch_size = 16
lr = 0.001
l2 = 0.001
"""
