import logging
from argparse import Namespace
from typing import Any

from learning.dataset import get_domain_file_from_opts, get_domain_from_opts
from learning.dataset.heuristic.container.cost_to_go_dataset import CostToGoDataset
from learning.dataset.heuristic.creator.classic_cost_to_go_dataset_creator import (
    ClassicCostToGoDatasetFromPlans,
    ClassicCostToGoDatasetFromStateSpace,
)
from learning.dataset.heuristic.creator.classic_ranking_dataset_creator import ClassicRankingDatasetFromPlans
from learning.dataset.heuristic.creator.numeric_cost_to_go_dataset_creator import NumericCostToGoDatasetFromPlans
from learning.dataset.heuristic.creator.numeric_ranking_dataset_creator import NumericRankingDatasetFromPlans
from learning.dataset.policy.dataset_labeller import DatasetLabeller
from learning.predictor.linear_model.predictor_factory import is_rank_predictor
from learning.predictor.neural_network.policy_type import PolicyType
from planning.util import is_domain_numeric
from wlplan.data import DomainDataset


def get_dataset(opts: Namespace) -> tuple[DomainDataset, Any]:
    """Collects dataset based on the type of learner.

    Args:
        opts (Namespace): parsed arguments

    Returns:
        tuple[DomainDataset, Any]: WLPlan dataset and labels.
    """

    if opts.policy_type is not None:
        return get_policy_dataset(opts)
    else:
        return get_heuristic_dataset(opts)


def get_heuristic_dataset(opts: Namespace) -> tuple[DomainDataset, Any]:
    """Collects dataset for heuristic learners.
    State space datasets automatically remove WL-indistinguishable states with equivalent target values.

    Args:
        opts (Namespace): parsed arguments

    Returns:
        tuple[DomainDataset, Any]: WLPlan dataset and labels.
    """

    is_rank = is_rank_predictor(opts.optimisation)
    data_generation = opts.data_generation
    is_numeric = is_domain_numeric(get_domain_file_from_opts(opts))

    if is_numeric and opts.facts != "nfd":
        logging.info("Changing facts option to 'nfd' for numeric planning.")
        opts.facts = "nfd"

    match (is_rank, data_generation, is_numeric):
        # Classic datasets
        case (False, "plan", False):
            ret = ClassicCostToGoDatasetFromPlans(opts).get_dataset()
        case (False, "state-space", False):
            ret = ClassicCostToGoDatasetFromStateSpace(opts).get_dataset()
        case (False, "all", False):
            dataset_1 = ClassicCostToGoDatasetFromPlans(opts).get_dataset()
            dataset_2 = ClassicCostToGoDatasetFromStateSpace(opts).get_dataset()
            dataset = CostToGoDataset(
                wlplan_domain=dataset_1.domain,
                data=dataset_1.data + dataset_2.data,
                y=dataset_1.y + dataset_2.y,
            )
            ret = dataset
        case (True, "plan", False):
            ret = ClassicRankingDatasetFromPlans(opts).get_dataset()
        case (True, _, False):
            raise ValueError("Ranking dataset from state space not supported as it is too large.")

        # Numeric datasets
        case (False, "plan", True):
            ret = NumericCostToGoDatasetFromPlans(opts).get_dataset()
        case (True, "plan", True):
            ret = NumericRankingDatasetFromPlans(opts).get_dataset()
        case (_, _, True):
            raise ValueError("Numeric dataset from state space not supported.")

        # Remaining problems
        case _:
            raise ValueError(f"Dataset configuration not supported {is_rank=}, {data_generation=}, {is_numeric=}")

    wlplan_dataset = ret.wlplan_dataset
    labels = ret.y

    return wlplan_dataset, labels


# def get_policy_dataset(policy_type: str, domain: Domain, dataset: LabelledDataset) -> tuple[DomainDataset, Any]:
def get_policy_dataset(opts: Namespace) -> tuple[DomainDataset, Any]:
    """Collects dataset for policy learners.

    Args:
        opts (Namespace): parsed arguments

    Returns:
        tuple[DomainDataset, Any]: WLPlan dataset and labels.
    """

    dataset_getter = DatasetLabeller(opts)

    policy_type = opts.policy_type
    match policy_type:
        case PolicyType.VALUE_FUNCTION.value:
            ret = dataset_getter.get_value_function_dataset()
        case PolicyType.QUALITY_FUNCTION.value:
            ret = dataset_getter.get_quality_function_dataset()
        case PolicyType.ADVANTAGE_FUNCTION.value:
            ret = dataset_getter.get_advantage_function_dataset()
        case PolicyType.POLICY_FUNCTION.value:
            ret = dataset_getter.get_policy_function_dataset(est=False)
        case PolicyType.POLICY_FUNCTION_X.value:
            ret = dataset_getter.get_policy_function_dataset(est=True)
        case _:
            raise ValueError(f"Unknown policy type: {policy_type}")

    wlplan_dataset, labels = ret

    return wlplan_dataset, labels
