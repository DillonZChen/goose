from typing import Optional

from _succgen.planning import StateStorer, VisitedStorage

from succgen.planning.node import SearchNode
from succgen.planning.state import State


class VisitedStorage:
    def __init__(self):
        self._states = StateStorer()
        self._i_to_node: dict[int, SearchNode] = dict()

    def add(self, node: SearchNode) -> None:
        state = node.state
        s_id = node.s_id
        self._states.add(state)
        self._i_to_node[s_id] = node

    def get(self, s_id: int) -> Optional[State]:
        return self._i_to_node.get(s_id)

    def contains(self, state: State) -> bool:
        return self._states.contains(state)
