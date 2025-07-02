import logging
from argparse import Namespace
from typing import Any

from learning.dataset import get_domain_file_from_opts
from learning.dataset.creator.classic_labelled_dataset_creator import LabelledDataset
from learning.predictor.linear_model.predictor_factory import is_rank_predictor
from learning.predictor.neural_network.policy_type import PolicyType
from planning.util import is_domain_numeric
from wlplan.data import DomainDataset, ProblemDataset
from wlplan.planning import Domain, Problem, State

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


def get_policy_dataset(policy_type: str, domain: Domain, dataset: LabelledDataset) -> tuple[DomainDataset, Any]:
    match policy_type:
        case PolicyType.VALUE_FUNCTION.value:
            return _get_value_function_dataset(domain, dataset)
        case PolicyType.QUALITY_FUNCTION.value:
            return _get_quality_function_dataset(domain, dataset)
        case PolicyType.ADVANTAGE_FUNCTION.value:
            return _get_advantage_function_dataset(domain, dataset)
        case _:
            raise ValueError(f"Unknown policy type: {policy_type}")


def _get_value_function_dataset(domain: Domain, dataset: LabelledDataset) -> tuple[DomainDataset, Any]:
    problem_dataset_list = []
    labels = []

    for data in dataset:
        problem = data.problem
        states = []
        for state_and_successors in data.states_and_successors_labelled:
            states.append(state_and_successors.state)  # s
            labels.append(state_and_successors.value)  # V(s)
            for successors in state_and_successors.successors_labelled:
                succ = successors.successor_state  # s
                value = successors.value  # V(s)
                if value is not None:
                    states.append(succ)
                    labels.append(value)
        problem_dataset_list.append(ProblemDataset(problem=problem, states=states))
    domain_dataset = DomainDataset(domain=domain, data=problem_dataset_list)

    return domain_dataset, labels


def _get_quality_function_dataset(domain: Domain, dataset: LabelledDataset) -> tuple[DomainDataset, Any]:
    problem_dataset_list = []
    labels = []

    for data in dataset:
        problem = data.problem
        states = []
        actions = []
        for state_and_successors in data.states_and_successors_labelled:
            state = state_and_successors.state  # s
            value = state_and_successors.value  # do nothing with the current value
            for successors in state_and_successors.successors_labelled:
                succ_state = successors.successor_state  # do nothing with the succ_state
                succ_value = successors.value  # use the succ value as Q(s, a)
                action = successors.action  # a
                if succ_value is not None:
                    states.append(state)
                    actions.append([action])
                    labels.append(succ_value)
        problem_dataset_list.append(ProblemDataset(problem=problem, states=states, actions=actions))
    domain_dataset = DomainDataset(domain=domain, data=problem_dataset_list)

    return domain_dataset, labels


def _get_advantage_function_dataset(domain: Domain, dataset: LabelledDataset) -> tuple[DomainDataset, Any]:
    problem_dataset_list = []
    labels = []

    for data in dataset:
        problem = data.problem
        states = []
        actions = []
        for state_and_successors in data.states_and_successors_labelled:
            state = state_and_successors.state  # s
            value = state_and_successors.value  # V(s)
            for successors in state_and_successors.successors_labelled:
                succ_state = successors.successor_state  # do nothing with the succ_state
                succ_value = successors.value  # use the succ value as Q(s, a)
                action = successors.action  # a
                if succ_value is not None:
                    states.append(state)
                    actions.append([action])
                    labels.append(succ_value - value)  # A(s, a) = Q(s, a) - V(s)
        problem_dataset_list.append(ProblemDataset(problem=problem, states=states, actions=actions))
    domain_dataset = DomainDataset(domain=domain, data=problem_dataset_list)

    return domain_dataset, labels
