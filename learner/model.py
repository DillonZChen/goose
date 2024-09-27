"""
Big wrapper for the NN model and the representation. It is also the object that
is called from NFD (c++) through pybind.
"""

import json
import logging
import pickle
import traceback
from abc import ABC, abstractmethod
from argparse import Namespace
from pprint import pprint
from typing import Dict, List, Optional, Tuple

import joblib
import torch

from learner.problem.numeric_problem import NumericProblem
from learner.problem.util import var_to_objects, var_to_predicate

logging.basicConfig(
    level=logging.INFO,
    # level=logging.DEBUG,
    format="%(levelname)s [%(filename)s:%(lineno)s] %(message)s",
)


class Model(ABC):
    def __init__(self) -> None:
        self.domain_pddl: str = None
        self.problem_pddl: str = None
        self.problem: NumericProblem = None
        self.opts: Namespace = None
        self.device = torch.device("cpu")

    @property
    def model_method(self) -> str:
        if not hasattr(self, "opts") or self.opts is None:
            return "wlf"  # this occurs from combining models
        return self.opts.model_method

    @property
    def model_name(self) -> str:
        if hasattr(self, "_model_name") or self._model_name is None:
            return self._model_name
        return self.opts.estimator_name

    @staticmethod
    def _get_device(device: Optional[int] = None):
        if torch.cuda.is_available() and device is None:
            dev = f"cuda"
        elif torch.cuda.is_available() and device is not None:
            dev = f"cuda:{device}"
        else:
            dev = "cpu"
        return torch.device(dev)

    def _set_device(self, device: int) -> None:
        self.device = self._get_device(device)

    def save(self, save_file: str, model_name: Optional[str] = None) -> None:
        # Previously, we used a zip file to be more efficient and decouple
        # the weights. However, running jobs in parallel causes issues.
        if model_name is not None:
            self._model_name = model_name
        torch.save(self, save_file)
        logging.info(f"Model saved successfully at {save_file}")

    def load(self, load_file: str) -> None:
        # Derived versions of this load is called from NFD
        try:
            obj = torch.load(load_file, map_location=self.device)
            for var, val in vars(obj).items():
                if var == "device":
                    continue
                setattr(self, var, val)
        except Exception as e:
            print("", flush=True)
            traceback.print_exc()
            print("", flush=True)
            exit(-1)

    @staticmethod
    def load_static(load_file: str) -> None:
        ## This one is called from run_ngoose.py mainly just to find out NFD arguments
        return torch.load(load_file, map_location=Model._get_device())

    @property
    @abstractmethod
    def multi_heuristics(self) -> bool:
        raise NotImplementedError

    @property
    @abstractmethod
    def pref_schema(self) -> bool:
        raise NotImplementedError

    @abstractmethod
    def dump(self) -> None:
        raise NotImplementedError

    """ All methods below are called only in cpp """

    def set_domain_problem(
        self, domain_pddl: str, problem_pddl: str, nfd_vars_string: str
    ) -> None:
        try:
            logging.info(f"Setting domain and problem...")
            self.domain_pddl = domain_pddl
            self.problem_pddl = problem_pddl
            self.problem = NumericProblem(
                domain_pddl,
                problem_pddl,
                self.opts,
                nfd_vars_string=nfd_vars_string,
            )
            logging.info(f"n_objects: {len(self.problem.objects)}")
        except:
            traceback.print_exc()
            print("", flush=True)
            exit(-1)

    def get_fluents(self) -> List[str]:
        return self._nfd_fluents

    def set_fluents(self, fluents: List[str]) -> None:
        # important to do this to preserve ordering of fluents that is used in NFD
        self._nfd_fluents = fluents

    def _print_fluents(self) -> None:
        for v in sorted(self._nfd_fluents):
            print(v)

    def get_fact_to_pred_objects(
        self, facts: List[str]
    ) -> Dict[str, Tuple[str, List[str]]]:
        ret = {}
        for fact in facts:
            pred = var_to_predicate(fact)
            objects = var_to_objects(fact)
            ret[fact] = (pred, objects)
        return ret

    @abstractmethod
    def evaluate(self, true_bools: List[str], num_vals: List[float]) -> float:
        raise NotImplementedError

    @abstractmethod
    def evaluate_batch(
        self, list_true_bools: List[List[str]], list_num_vals: List[List[float]]
    ) -> List[float]:
        raise NotImplementedError
