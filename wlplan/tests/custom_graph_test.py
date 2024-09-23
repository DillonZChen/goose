import logging
import random

import networkx as nx
import numpy as np
from ipc23lt import get_raw_dataset

from wlplan.feature_generation import WLFeatures
from wlplan.graph import from_networkx

LOGGER = logging.getLogger(__name__)


def test_blocksworld_random_path():
    random.seed(0)
    LOGGER.info("Getting raw dataset")
    domain, dataset, _ = get_raw_dataset(domain_name="blocksworld", keep_statics=False)
    LOGGER.info("Constructing feature generator")
    feature_generator = WLFeatures(
        domain=domain,
        graph_representation=None,
        iterations=4,
        prune_features=None,
    )
    graphs = []
    LOGGER.info("Converting to random path graphs")
    for _, states in dataset:
        for state in states:
            G = nx.Graph()
            for i, atom in enumerate(state):
                G.add_node(str(atom), colour=random.randint(0, 10))
                if i > 0:
                    G.add_edge(str(state[i - 1]), str(atom), label=random.randint(0, 10))
            G = from_networkx(G)
            graphs.append(G)
            G.dump()
    LOGGER.info("Collecting features")
    feature_generator.collect(graphs)
    LOGGER.info("Embedding")
    X = np.array(feature_generator.embed(graphs)).astype(float)
    n_features = feature_generator.get_n_features()
    assert X.shape[1] == n_features
    LOGGER.info(f"{n_features} features collected from random path graphs")
