from learner.dataset.blank_dataset import BlankDataset
from learner.dataset.dataset import Dataset
from learner.dataset.deadend_dataset import DeadendDataset
from learner.dataset.heuristic_dataset import HeuristicDataset
from learner.dataset.preferred_schema_dataset import PreferredSchemaDataset
from learner.dataset.ranking_dataset import RankingDataset

DATASETS = {
    "h": HeuristicDataset,
    "d": DeadendDataset,
    "p": PreferredSchemaDataset,
    "r": RankingDataset,
    None: BlankDataset,
}

def dataset_options():
    return DATASETS.keys()

def dataset_class(target: str) -> Dataset:
    return DATASETS[target]
