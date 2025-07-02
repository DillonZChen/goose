from dataclasses import dataclass
from typing import Optional

from _succgen.planning import Node as SearchNode

from succgen.planning.state import State

__all__ = ["SearchNode"]


@dataclass
class SearchNode:
    state: State
    achieving_action: Optional[str]
    s_id: int
    parent_s_id: Optional[int]
    g: int
