import random
from abc import ABC, abstractmethod

from succgen.planning.action import SGAction
from succgen.planning.state import State
from succgen.planning.task import Task


class Policy(ABC):
    def __init__(self, task: Task):
        self._task = task

    def generate(self, state: State, applicable_actions: list[SGAction]) -> SGAction:
        # readable = {pred: set() for pred in self._task.schema_to_i.keys()}
        # for action in applicable_actions:
        #     predicate, args = self._task.action_to_readable(action)
        #     readable[predicate].add(args)
        # action = self.select(state, readable)
        # action = (self._task.schema_to_i[action[0]], tuple(self._task.obj_to_i[obj] for obj in action[1]))

        i_to_a = []
        a_to_i = {}
        for i, action in enumerate(applicable_actions):
            action = self._task.action_to_string(action).replace("(", "").replace(")", "")
            a_to_i[action] = i
            i_to_a.append(action)

        action = self.select(state, i_to_a)
        if action in a_to_i:
            action = applicable_actions[a_to_i[action]]
        else:
            action = random.choice(applicable_actions)

        return action

    @abstractmethod
    def select(self, state: State, actions):
        """
        Select an action from the list of actions based on the policy.

        Args:
            state (State): The current state.
            actions (list[ReadableAction]): The list of actions to choose from.

        Returns:
            ReadableAction: The selected action.
        """
        raise NotImplementedError("Policy selection not implemented.")
