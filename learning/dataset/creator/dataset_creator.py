import argparse
import os
from abc import abstractmethod

from tqdm import tqdm

import wlplan
from learning.dataset import (
    get_domain_file_from_opts,
    get_training_dir_from_opts,
    get_training_plans_dir_from_opts,
)
from learning.dataset.container.base_dataset import Dataset
from util.error_message import get_path_error_msg

MAX_EXPANSIONS_PER_PROBLEM = 10000
MAX_STATE_SPACE_DATA = 100000


class DatasetCreator:
    def __init__(self, opts: argparse.Namespace):
        # domain information
        self.domain_pddl = get_domain_file_from_opts(opts)
        self.tasks_dir = get_training_dir_from_opts(opts)

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
            plans_dir = get_training_plans_dir_from_opts(self._opts)
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
