import os

import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
from domains import DOMAINS

PLOT_DIR = "_plots"
os.makedirs(PLOT_DIR, exist_ok=True)

for domain in DOMAINS:
    c_plans = f"{domain}/classic/training_plans/"
    n_plans = f"{domain}/numeric/training_plans/"

    data = {"c": [], "n": [], "p": []}

    for p in sorted(os.listdir(c_plans)):
        c_f = f"{c_plans}{p}"
        n_f = f"{n_plans}{p}"
        if not os.path.exists(n_f):
            continue
        c_l = 0
        n_l = 0
        p = p.replace(".plan", "")
        for line in open(c_f, "r").readlines():
            if line.startswith(";"):
                c_l = int(line.split()[3])
                break
        for line in open(n_f, "r").readlines():
            if line.startswith(";"):
                n_l = int(line.split()[3])
                break
        data["c"].append(c_l)
        data["n"].append(n_l)
        data["p"].append(p)

    if len(data["c"]) == 0:
        continue

    max_x = max(max(data["c"]), max(data["n"]))
    fig = px.scatter(
        data_frame=pd.DataFrame(data),
        x="c",
        y="n",
        range_x=[0, max_x],
        range_y=[0, max_x],
        hover_data=["p"],
        title=f"{len(data['c'])} optimal plans collected",
    )
    fig.add_trace(go.Scatter(x=[0, max_x], y=[0, max_x], mode="lines"))
    fig.write_html(f"{PLOT_DIR}/{domain}.html")
    fig.write_image(f"{PLOT_DIR}/{domain}.png")
    print(f"Written plot to {PLOT_DIR}/{domain}.html")
