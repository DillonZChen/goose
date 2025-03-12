import os
import sys
from abc import abstractmethod

import toml
from tqdm import tqdm

import wlplan
from learning.dataset.container.base_dataset import Dataset
from util.error_message import get_path_error_msg
from wlplan.feature_generation import Features

MAX_EXPANSIONS_PER_PROBLEM = 10000
MAX_STATE_SPACE_DATA = 100000


class DatasetCreator:
    def __init__(
        self,
        data_config: str,
        feature_generator: Features,
        hash_prefix: str,
    ):
        # domain information
        data_config = toml.load(data_config)

        self.domain_pddl = data_config["domain_pddl"]
        self.tasks_dir = data_config["tasks_dir"]
        # plans_dir collected later as not always necessary (e.g. state space data)
        self._data_config = data_config

        assert os.path.exists(self.domain_pddl), get_path_error_msg(self.domain_pddl)
        assert os.path.exists(self.tasks_dir), get_path_error_msg(self.tasks_dir)

        self.wlplan_domain = wlplan.planning.parse_domain(self.domain_pddl)

        # feature generator
        self.feature_generator = feature_generator

        # prevent tmp files from being overwritten by parallel jobs
        self.hash_prefix = hash_prefix

    def _get_problem_iterator(self, plans_only=True):
        pbar = []
        if not plans_only:
            pbar = [self.tasks_dir + "/" + f for f in sorted(os.listdir(self.tasks_dir))]
        else:
            self.plans_dir = self._data_config["plans_dir"]
            assert os.path.exists(self.plans_dir), get_path_error_msg(self.plans_dir)
            for f in sorted(os.listdir(self.plans_dir)):
                problem_pddl = self.tasks_dir + "/" + f.replace(".plan", ".pddl")
                plan_file = self.plans_dir + "/" + f
                pbar.append((problem_pddl, plan_file))
        pbar = tqdm(pbar, desc="Collecting data from problems")
        return pbar

    @abstractmethod
    def get_dataset(self) -> Dataset:
        pass
