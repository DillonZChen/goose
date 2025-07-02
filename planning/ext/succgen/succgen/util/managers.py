import logging
import time

import termcolor as tc


class LoggerManager:
    def __init__(self, log_level):
        self._log_level = log_level

    def __enter__(self):
        self._current_level = logging.getLogger().getEffectiveLevel()
        logging.getLogger().setLevel(self._log_level)

    def __exit__(self, exc_type, exc_value, traceback):
        if traceback is not None:
            return
        logging.getLogger().setLevel(self._current_level)


class TimerContextManager:
    def __init__(self, description: str | None = None, for_debug: bool = False):
        self.description = description
        self.debug = for_debug

    def log(self, msg):
        if self.description is None:
            return
        if self.debug:
            logging.debug(msg)
        else:
            logging.info(msg)

    def __enter__(self):
        msg = tc.colored(f"Started {self.description}...", "magenta")
        self.log(msg)
        self.start_time = time.time()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        if traceback is not None:
            return
        end_time = time.time()
        execution_time = end_time - self.start_time
        if not self.description:
            return
        msg = tc.colored(f"Finished {self.description} in {execution_time}s", "green")
        self.log(msg)

    def get_time(self):
        return time.time() - self.start_time
