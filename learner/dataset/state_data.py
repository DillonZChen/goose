from dataclasses import dataclass
from typing import List, Optional

from learner.problem.numeric_state import NumericState


@dataclass
class StateData:
    state: NumericState
    parent_state: Optional[NumericState]
    description: str
    heuristic: float
    optimal_actions: Optional[List[str]]
