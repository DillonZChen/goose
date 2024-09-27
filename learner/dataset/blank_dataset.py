""" used mainly for dealing with raw dataset and not for training, e.g. dump to json """

from typing import Dict

from learner.dataset.dataset import Dataset


class BlankDataset(Dataset):
    def get_dataset_split(self):
        return []

    def get_metrics(self) -> Dict[str, callable]:
        return {}
