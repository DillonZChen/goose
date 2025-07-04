import json
from dataclasses import dataclass
from typing import Optional

from wlplan.planning import Action, Problem, State


@dataclass
class LabelledSuccessorData:
    action: Action
    successor_state: State
    value: Optional[int]


@dataclass
class LabelledStateAndSuccessorsData:
    state: State
    value: int
    successors_labelled: list[LabelledSuccessorData]

    def __repr__(self):
        return json.dumps(
            {
                "s": str(self.state),
                "value": self.value,
                "successors": [
                    {
                        "a": str(ss.action),
                        "s'": str(ss.successor_state),
                        "v": ss.value,
                    }
                    for ss in self.successors_labelled
                ],
            },
            indent=2,
        )


@dataclass
class LabelledProblemData:
    problem: Problem
    states_and_successors_labelled: list[LabelledStateAndSuccessorsData]


LabelledDataset = list[LabelledProblemData]
