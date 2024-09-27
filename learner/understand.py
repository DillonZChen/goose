import os

import numpy as np
from pyvis.network import Network

from learner.model import Model


def understand_model(model: Model, model_path: str, X: np.array) -> None:
    ## use save file as easy descriptor
    dirr = f"_understand_graphs/{os.path.basename(model_path).split('.')[0]}"
    os.makedirs(dirr, exist_ok=True)

    # model: FeatureGenerationModel = Model.load_static(model_path)

    ### some preprocessing of model attributes
    model.representation.dump()

    weights = model.get_weights()
    weights = weights[0]

    features = model.representation.feature_generator.features()
    i_to_features = {i: f for f, i in features}

    n_weights = len(weights)
    assert n_weights % 2 == 0
    n_cat = n_weights // 2

    non_zero_weight_idx = np.where(weights != 0)[0].tolist()
    non_zero_weights = weights[non_zero_weight_idx].tolist()

    wl_hash = model.get_hash()

    ## remap identifies the base feature in NILG of a colour if it exists
    remap = {}
    i_to_parents = {}
    for v, k in wl_hash.items():
        k = int(k)
        if "," not in v:
            v = int(v)
            remap[k] = i_to_features[v]

    def transform_tok(tok):
        ret = tok
        if tok in remap:
            ret = remap[tok]
        return ret

    for v, k in wl_hash.items():
        k = int(k)
        if "," in v:
            toks = v.split(",")
            toks = [int(t) for t in toks]
            parents = [transform_tok(toks[0])]
            assert len(toks) % 2 == 1
            for i in range(1, len(toks), 2):
                parents.append(
                    (transform_tok(toks[i]), int(toks[i + 1]))
                )  # feat, label
            i_to_parents[k] = parents
    for i, parents in i_to_parents.items():
        print(i, parents)

    ### construct graph from a feature and save
    def graph(idx, desc):
        N = Network(height="1350px", width="100%", notebook=True, directed=True)
        N.toggle_hide_edges_on_drag(False)
        N.toggle_hide_nodes_on_drag(False)
        N.barnes_hut()

        seen = set()
        i = idx
        q = []
        if i in remap:
            i = remap[i]
            N.add_node(i, label=str(i), color="gold")
        elif i in i_to_features:
            i = i_to_features[i]
            N.add_node(i, label=str(i), color="gold")
        else:
            aggr = str(i_to_parents[i]).replace("'", "")
            node_label = f"{i}:{aggr}"
            N.add_node(i, label=node_label, color="gold")
            q.append(i)

        def add_node(n):
            nonlocal N
            if n in i_to_parents:
                aggr = str(i_to_parents[n]).replace("'", "")
                node_label = f"{n}:{aggr}"
            else:
                node_label = n
            N.add_node(n, label=node_label)

        while q:
            i = q.pop(0)
            if i in i_to_parents:
                parents = i_to_parents[i]
                for p in parents:
                    if isinstance(p, tuple):
                        n = p[0]
                        add_node(n)
                        N.add_edge(n, i, label=str(p[1]), title=str(p[1]))
                    else:
                        n = p
                        add_node(n)
                        N.add_edge(n, i, label="self", title="self")
                    q.append(n)
                    seen.add(n)

        _size = 100
        for node in N.nodes:
            node["size"] = _size
            node["font"] = {"size": _size * 0.66}
        for edge in N.edges:
            edge["font"] = {"size": _size * 0.66}

        N.show(f"{dirr}/{desc}.html")

    ### estimate same feature groups from X
    feature_to_is = {}
    i_to_group = {}
    i_to_feature = {}
    for i in range(len(X[0])):
        x = tuple(X[:, i])
        if x not in feature_to_is:
            feature_to_is[x] = []
        feature_to_is[x].append(i)
        i_to_feature[i] = tuple(x)
    for f, is_ in feature_to_is.items():
        for i in is_:
            i_to_group[i] = is_

    ### visualise
    selected_groups = set()
    cnt = {}
    feature_representer = {}
    for idx, w in zip(non_zero_weight_idx, non_zero_weights):
        # idx = int(idx.split("_")[1])
        f = i_to_feature[idx] 
        if idx not in selected_groups:
            for i in i_to_group[idx]:
                selected_groups.add(i)
            if f in cnt:
                breakpoint()
            assert f not in cnt
            cnt[f] = 1
            feature_representer[f] = (idx, w)
        else:
            if f not in cnt:
                breakpoint()
            assert f in cnt
            cnt[f] += 1
            assert feature_representer[f][1] == w


    # for i, val in enumerate(non_zero_weight_idx):
    #     if val < n_cat:
    #         non_zero_weight_idx[i] = f"b_{val}"
    #     else:
    #         non_zero_weight_idx[i] = f"n_{val - n_cat}"

    for f, (idx, w) in feature_representer.items():
        w = w * cnt[f]
        if idx < n_cat:
            desc = f"b_{idx}_{w}"
        else:
            idx = idx - n_cat
            desc = f"n_{idx}_{w}"
        graph(idx, desc)
