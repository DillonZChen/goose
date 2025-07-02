import logging

from succgen.planning.action import SGAction
from succgen.planning.state import SGState
from typing_extensions import override

from learning.predictor.neural_network.serialise import load_gnn
from planning.policy.policy import PolicyExecutor
from wlplan.planning import Action, State


class GnnPolicyExecutor(PolicyExecutor):
    def __init__(
        self,
        domain_file: str,
        problem_file: str,
        model_file: str,
        debug: bool = False,
        bound: int = -1,
    ):
        super().__init__(domain_file=domain_file, problem_file=problem_file, debug=debug, bound=bound)
        self._model, config_opts = load_gnn(model_file)
        self._policy_type = config_opts.policy_type
        logging.info(f"{self._policy_type=}")

    @override
    def select_action(self, state: SGState, actions: list[SGAction]) -> SGAction:
        # to wl state

        state = self._task.dump_state(state)
        print(f"{state=}")
        for action in actions:
            print(f"{action=}")
        raise NotImplementedError
