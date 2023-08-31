from typing import List

StripsState = List[bool]

class GoalCountHeuristic():
  def __init__(self, goal: StripsState) -> None:
    self.goal = goal

  def h(self, state: StripsState) -> float:
    goals_remaining = len(set(self.goal).difference(set(state)))
    return goals_remaining

  def h_batch(self, states: List[StripsState]) -> float:
    return [self.h(state) for state in states]
  