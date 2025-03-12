#!/usr/bin/env python3

import os
import re

import pandas as pd

CUR_DIR = os.path.dirname(os.path.realpath(__file__))
LOG_DIR = os.path.join(CUR_DIR, "_collect_logs")

keys = ["domain", "plan_generation", "pruning", "refined_colours", "0", "1", "2", "3", "4"]
data = {k: [] for k in keys}
for f in sorted(os.listdir(LOG_DIR)):
    if f.endswith(".log"):
        with open(os.path.join(LOG_DIR, f), "r") as file:
            content = file.read()
        f = f.replace(".log", "")
        domain, plan_generation, pruning = f.split("_")
        data["domain"].append(domain)
        data["plan_generation"].append(plan_generation)
        data["pruning"].append(pruning)
        # [INFO t=10.4892s] n_refined_colours=123
        # [INFO t=10.4893s]   0=10
        # [INFO t=10.4893s]   1=21
        # [INFO t=10.4893s]   2=33
        # [INFO t=10.4893s]   3=43
        # [INFO t=10.4893s]   4=55
        refined_colours = re.search(r"n_refined_colours=(\d+)", content)
        if refined_colours is None:
            refined_colours = -1
        else:
            refined_colours = int(refined_colours.group(1))
        data["refined_colours"].append(refined_colours)
        for i in range(5):
            colour = re.search(rf"{i}=(\d+)", content)
            if colour is None:
                colour = -1
            else:
                colour = int(colour.group(1))
            data[str(i)].append(colour)
df = pd.DataFrame(data)
print(df)
