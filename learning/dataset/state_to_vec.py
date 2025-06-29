from argparse import Namespace

import numpy as np
from tqdm import tqdm

from learning.dataset.container.base_dataset import Dataset
from learning.dataset.container.ranking_dataset import RankingDataset
from learning.predictor.predictor_factory import is_rank_predictor
from wlplan.feature_generation import Features


def embed_data(dataset: Dataset, wlf_generator: Features, opts: Namespace):
    if opts.data_pruning == "none":
        X = wlf_generator.embed(dataset.wlplan_dataset)
        X = np.array(X).astype(float)
        y = dataset.y
        sample_weight = None
    elif opts.data_pruning == "equivalent-weighted":
        X, y, sample_weight = get_data_weighted(dataset, wlf_generator, opts)
    elif opts.data_pruning == "equivalent":
        X, y, _ = get_data_weighted(dataset, wlf_generator, opts)
        sample_weight = None
    else:
        raise ValueError(f"Unknown data pruning method: {opts.data_pruning}")
    return X, y, sample_weight


def get_data_weighted(dataset: Dataset, feature_generator: Features, opts: Namespace):
    if is_rank_predictor(opts.optimisation):
        assert isinstance(dataset, RankingDataset)
        dataset: RankingDataset = dataset
        unique_groups = {}
        sample_weight_dict = {}
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
                sample_weight_dict[key] = 0
            sample_weight_dict[key] += 1
        X = []
        y = []
        sample_weight = []
        for key, ranking_group in unique_groups.items():
            for i in range(3):
                for x in key[i]:
                    X.append(x)
            y.append(ranking_group)
            sample_weight.append(sample_weight_dict[key])
        X = np.array(X).astype(float)
        return X, y, sample_weight
    else:
        unique_rows = {}
        graphs = feature_generator.convert_to_graphs(dataset.wlplan_dataset)
        for graph, y in tqdm(zip(graphs, dataset.y), total=len(graphs)):
            x = feature_generator.embed(graph)
            xy = np.array(x + [y])
            xy = tuple(xy)
            if xy not in unique_rows:
                unique_rows[xy] = 0
            unique_rows[xy] += 1
        Xy = []
        sample_weight = []
        for xy, count in unique_rows.items():
            Xy.append(xy)
            sample_weight.append(count)
        Xy = np.array(Xy).astype(float)
        sample_weight = np.array(sample_weight).astype(float)
        assert Xy.shape[0] == sample_weight.shape[0]
        return Xy[:, :-1], Xy[:, -1], sample_weight
