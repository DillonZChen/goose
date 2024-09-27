from itertools import product

from configs import h_gnn, h_wlf, r_gnn, r_wlf

CONFIGS = [
    (h_gnn, "h-gnn"),
    (r_gnn, "r-gnn"),
    (h_wlf, "h-wlf"),
    (r_wlf, "r-wlf"),
]

for (config, name), layers in product(CONFIGS, range(5)):
    with open(f"{name}{layers}.toml", "w") as f:
        config = config.replace("<LAYERS>", str(layers))
        f.write(config)
