import logging
from argparse import Namespace

import toml

from planning.util import is_numeric
from wlplan.feature_generation import WLFeatures

from .container.base_dataset import Dataset
from .container.cost_to_go_dataset import CostToGoDataset
from .creator.classic_cost_to_go_dataset_creator import (ClassicCostToGoDatasetFromPlans,
                                                         ClassicCostToGoDatasetFromStateSpace)
from .creator.classic_ranking_dataset_creator import ClassicRankingDatasetFromPlans
from .creator.numeric_cost_to_go_dataset_creator import NumericCostToGoDatasetFromPlans
from .creator.numeric_ranking_dataset_creator import NumericRankingDatasetFromPlans

DOMAINS = {
    "blocksworld",
    "childsnack",
    "ferry",
    "floortile",
    "miconic",
    "rovers",
    "satellite",
    "sokoban",
    "spanner",
    "transport",
}


def get_dataset(opts: Namespace, feature_generator: WLFeatures) -> Dataset:
    """State space datasets automatically remove WL-indistinguishable states with equivalent target values."""

    rank = opts.rank
    data_generation = opts.data_generation
    domain_is_numeric = is_numeric(toml.load(opts.data_config)["domain_pddl"])
    if domain_is_numeric:
        if opts.facts != "nfd":
            logging.info("Changing facts option to 'nfd' for numeric planning.")
        opts.facts = "nfd"

    kwargs = {
        "feature_generator": feature_generator,
        "data_config": opts.data_config,
        "facts": opts.facts,
        "hash_prefix": str(hash(repr(opts))),
    }

    rank_ss_err_msg = "Ranking dataset from state space not supported as it is too large."
    numeric_ss_err_msg = "Numeric dataset from state space not implemented."

    match (rank, data_generation, domain_is_numeric):
        ##### Classic datasets #####
        case (False, "plan", False):
            return ClassicCostToGoDatasetFromPlans(**kwargs).get_dataset()
        case (False, "state-space", False):
            return ClassicCostToGoDatasetFromStateSpace(**kwargs).get_dataset()
        case (False, "all", False):
            dataset_1 = ClassicCostToGoDatasetFromPlans(**kwargs).get_dataset()
            dataset_2 = ClassicCostToGoDatasetFromStateSpace(**kwargs).get_dataset()
            dataset = CostToGoDataset(
                wlplan_domain=dataset_1.domain,
                data=dataset_1.data + dataset_2.data,
                y=dataset_1.y + dataset_2.y,
            )
            return dataset
        case (True, "plan", False):
            return ClassicRankingDatasetFromPlans(**kwargs).get_dataset()
        case (True, _, False):
            raise ValueError(rank_ss_err_msg)

        ##### Numeric datasets #####
        case (False, "plan", True):
            return NumericCostToGoDatasetFromPlans(**kwargs).get_dataset()
        case (True, "plan", True):
            return NumericRankingDatasetFromPlans(**kwargs).get_dataset()
        case (_, _, True):
            raise ValueError(numeric_ss_err_msg)

        ##### Remaining problems #####
        case _:
            raise ValueError(f"Unknown dataset configuration {rank=}, {data_generation=}, {domain_is_numeric=}")
