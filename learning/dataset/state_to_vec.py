from argparse import Namespace

import numpy as np
from tqdm import tqdm

from learning.dataset.container.base_dataset import Dataset
from learning.dataset.container.ranking_dataset import RankingDataset
from wlplan.feature_generation import Features


def embed_data(dataset: Dataset, feature_generator: Features, opts: Namespace):
    if opts.data_pruning == "none":
        X = feature_generator.embed(dataset.wlplan_dataset)
        X = np.array(X).astype(float)
        y = dataset.y
        return X, y
    if opts.rank:
        assert isinstance(dataset, RankingDataset)
        dataset: RankingDataset = dataset
        unique_groups = {}
        counter = 0
        graphs = feature_generator.convert_to_graphs(dataset.wlplan_dataset)
        for ranking_group in tqdm(dataset.y, total=len(dataset.y)):
            gg = ranking_group.good_group
            mg = ranking_group.maybe_group
            bg = ranking_group.bad_group
            good_graphs = [graphs[i] for i in gg]
            maybe_graphs = [graphs[i] for i in mg]
            bad_graphs = [graphs[i] for i in bg]
            good_x = feature_generator.embed(good_graphs)
            maybe_x = feature_generator.embed(maybe_graphs)
            bad_x = feature_generator.embed(bad_graphs)
            good_x = sorted(tuple(x) for x in good_x)
            maybe_x = sorted(tuple(x) for x in maybe_x)
            bad_x = sorted(tuple(x) for x in bad_x)
            key = (tuple(good_x), tuple(maybe_x), tuple(bad_x))
            if key not in unique_groups:
                ranking_group.good_group = [i + counter for i in range(len(gg))]
                counter += len(gg)
                ranking_group.maybe_group = [i + counter for i in range(len(mg))]
                counter += len(mg)
                ranking_group.bad_group = [i + counter for i in range(len(bg))]
                counter += len(bg)
                unique_groups[key] = ranking_group
        X = []
        y = []
        for key, ranking_group in unique_groups.items():
            for i in range(3):
                for x in key[i]:
                    X.append(x)
            y.append(ranking_group)
        X = np.array(X).astype(float)
        return X, y
    else:
        unique_rows = set()
        graphs = feature_generator.convert_to_graphs(dataset.wlplan_dataset)
        for graph, y in tqdm(zip(graphs, dataset.y), total=len(graphs)):
            x = feature_generator.embed(graph)
            xy = np.array(x + [y])
            unique_rows.add(tuple(xy))
        Xy = np.array(list(unique_rows)).astype(float)
        return Xy[:, :-1], Xy[:, -1]
