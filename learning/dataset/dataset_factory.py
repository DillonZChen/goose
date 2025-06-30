import logging
from argparse import Namespace

from learning.dataset import get_domain_file_from_opts
from learning.predictor.linear_model.predictor_factory import is_rank_predictor
from planning.util import is_domain_numeric

from .container.base_dataset import Dataset
from .container.cost_to_go_dataset import CostToGoDataset
from .creator.classic_cost_to_go_dataset_creator import (
    ClassicCostToGoDatasetFromPlans,
    ClassicCostToGoDatasetFromStateSpace,
)
from .creator.classic_ranking_dataset_creator import ClassicRankingDatasetFromPlans
from .creator.numeric_cost_to_go_dataset_creator import NumericCostToGoDatasetFromPlans
from .creator.numeric_ranking_dataset_creator import NumericRankingDatasetFromPlans


def get_dataset(opts: Namespace) -> Dataset:
    """State space datasets automatically remove WL-indistinguishable states with equivalent target values."""

    is_rank = is_rank_predictor(opts.optimisation)
    data_generation = opts.data_generation
    is_numeric = is_domain_numeric(get_domain_file_from_opts(opts))

    if is_numeric and opts.facts != "nfd":
        logging.info("Changing facts option to 'nfd' for numeric planning.")
        opts.facts = "nfd"

    match (is_rank, data_generation, is_numeric):
        # Classic datasets
        case (False, "plan", False):
            return ClassicCostToGoDatasetFromPlans(opts).get_dataset()
        case (False, "state-space", False):
            return ClassicCostToGoDatasetFromStateSpace(opts).get_dataset()
        case (False, "all", False):
            dataset_1 = ClassicCostToGoDatasetFromPlans(opts).get_dataset()
            dataset_2 = ClassicCostToGoDatasetFromStateSpace(opts).get_dataset()
            dataset = CostToGoDataset(
                wlplan_domain=dataset_1.domain,
                data=dataset_1.data + dataset_2.data,
                y=dataset_1.y + dataset_2.y,
            )
            return dataset
        case (True, "plan", False):
            return ClassicRankingDatasetFromPlans(opts).get_dataset()
        case (True, _, False):
            raise ValueError("Ranking dataset from state space not supported as it is too large.")

        # Numeric datasets
        case (False, "plan", True):
            return NumericCostToGoDatasetFromPlans(opts).get_dataset()
        case (True, "plan", True):
            return NumericRankingDatasetFromPlans(opts).get_dataset()
        case (_, _, True):
            raise ValueError("Numeric dataset from state space not supported.")

        # Remaining problems
        case _:
            raise ValueError(f"Dataset configuration not supported {is_rank=}, {data_generation=}, {is_numeric=}")
