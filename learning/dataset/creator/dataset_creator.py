import argparse
import os
from abc import abstractmethod

import toml
from tqdm import tqdm

import wlplan
from learning.dataset.container.base_dataset import Dataset
from util.error_message import get_path_error_msg
from wlplan.feature_generator import Features

MAX_EXPANSIONS_PER_PROBLEM = 10000
MAX_STATE_SPACE_DATA = 100000


class DatasetCreator:
    def __init__(self, opts: argparse.Namespace):
        # domain information
        data_config = toml.load(opts.data_config)

        self.domain_pddl = data_config["domain_pddl"]
        self.tasks_dir = data_config["tasks_dir"]
        # plans_dir collected later as not always necessary (e.g. state space data)
        self._data_config = data_config

        assert os.path.exists(self.domain_pddl), get_path_error_msg(self.domain_pddl)
        assert os.path.exists(self.tasks_dir), get_path_error_msg(self.tasks_dir)

        self._wlplan_domain = wlplan.planning.parse_domain(self.domain_pddl)

        # hack to prevent tmp files from being overwritten by parallel jobs
        self._hash_prefix = str(hash(repr(opts)))

        # number of data to collect
        self._num_data = opts.num_data

        self._opts = opts

    def _get_problem_iterator(self, plans_only=True):
        pbar = []
        if not plans_only:
            pbar = [self.tasks_dir + "/" + f for f in sorted(os.listdir(self.tasks_dir))]
        else:
            plans_dir = self._data_config["plans_dir"]
            assert os.path.exists(plans_dir), get_path_error_msg(plans_dir)
            for f in sorted(os.listdir(plans_dir)):
                problem_pddl = self.tasks_dir + "/" + f.replace(".plan", ".pddl")
                plan_file = plans_dir + "/" + f
                if not os.path.exists(plan_file) or not os.path.exists(problem_pddl):
                    continue
                pbar.append((problem_pddl, plan_file))

        if self._num_data is not None and self._num_data >= 0:
            pbar = pbar[: self._num_data]
        pbar = tqdm(pbar, desc="Collecting data from problems")

        return pbar

    @abstractmethod
    def get_dataset(self) -> Dataset:
        pass
