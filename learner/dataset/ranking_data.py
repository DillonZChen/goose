from dataclasses import dataclass
from typing import Dict, List, Tuple

from learner.problem.numeric_problem import NumericProblem
from learner.problem.numeric_state import NumericState


@dataclass
class RankingData:
    problem: NumericProblem
    states: List[NumericState]
    good_idxs: List[int]
    maybe_bad_idxs: List[int]
    def_bad_idxs: List[int]
