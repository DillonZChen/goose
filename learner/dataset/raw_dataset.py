from typing import Dict, List

from learner.dataset.state_data import StateData
from learner.problem.numeric_problem import NumericProblem

RawDataset = Dict[NumericProblem, List[StateData]]
