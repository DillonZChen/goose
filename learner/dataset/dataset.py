import json
import logging
import os
import pprint
from abc import ABC, abstractmethod
from argparse import Namespace
from typing import Dict, List

import torch
from sklearn.model_selection import train_test_split
from torch_geometric.loader import DataLoader
from tqdm import tqdm

from learner.dataset.raw_dataset import RawDataset
from learner.problem.numeric_domain import NumericDomain
from learner.problem.numeric_problem import NumericProblem
from learner.representation import Representation
from util.timer import TimerContextManager

ALL_KEY = "_all_"


# A dataset for that has already been processed for use for ML models
class Dataset(ABC):
    def __init__(self, opts: Namespace, representation: Representation) -> None:
        self.representation = representation
        self._opts = opts

        self.domain_pddl = opts.domain_pddl
        self.tasks_dir = opts.tasks_dir
        self.plans_dir = opts.plans_dir

        self._schemata_keys = {ALL_KEY}

        self.dataset: List = []

        with TimerContextManager("loading raw dataset"):
            self._raw_dataset: RawDataset = self.load_raw_dataset()

        if self._opts.model_method == "none":
            return

    def __len__(self) -> int:
        return len(self.dataset)

    def __getitem__(self, idx: int):
        return self.dataset[idx]

    @property
    def schemata_keys(self):
        return self._schemata_keys

    @abstractmethod
    def get_dataset_split(self):
        raise NotImplementedError

    @abstractmethod
    def get_metrics(self) -> Dict[str, callable]:
        raise NotImplementedError

    def load_raw_dataset(self) -> RawDataset:
        domain_pddl = self.domain_pddl
        tasks_dir = self.tasks_dir
        plans_dir = self.plans_dir

        ret: RawDataset = {}
        tasks = [f for f in os.listdir(plans_dir) if ".plan" in f]
        pbar = tqdm(sorted(tasks))
        for plan_f in pbar:
            pbar.set_description(f"Processing {plan_f}")
            problem_name = plan_f.replace(".plan", "")
            problem_pddl = f"{tasks_dir}/{problem_name}.pddl"
            plan_file = f"{plans_dir}/{problem_name}.plan"
            assert os.path.exists(problem_pddl), problem_pddl

            problem = NumericProblem(domain_pddl, problem_pddl, self._opts)

            states, actions = problem.trace_and_succs_from_plan_file(plan_file)

            ret[problem] = states

        return ret

    def get_loaders(self, opts, device):
        batch_size = opts.batch_size
        seed = opts.seed
        dataset = self.dataset

        # try put whole dataset in memory
        if device.type == "cuda":
            data_mem = 0
            for data in dataset:
                x = data.x
                edge_indices = data.edge_index
                data_mem += x.element_size() * x.nelement()
                data_mem += sum(e.element_size() * e.nelement() for e in edge_indices)

            gpu_mem = torch.cuda.get_device_properties(device).total_memory
            print(f"Data size: {float(data_mem)/1e9} GB")
            print(f"GPU size: {float(gpu_mem)/1e9} GB")

            if data_mem <= gpu_mem:
                with TimerContextManager("putting whole dataset to device"):
                    for i, data in enumerate(dataset):
                        dataset[i] = data.to(device)

        train_set, val_set = train_test_split(
            dataset, test_size=opts.val_ratio, random_state=seed
        )
        train_loader = DataLoader(train_set, batch_size=batch_size, shuffle=True)
        val_loader = DataLoader(val_set, batch_size=batch_size, shuffle=False)
        print("num_train:", len(train_set))
        print("num_val:", len(val_set))

        return train_loader, val_loader
