import logging
import time

from termcolor import colored


class TimerContextManager:
    def __init__(self, description):
        self.description = description

    def __enter__(self):
        logging.info(colored(f"Started {self.description}...", "magenta"))
        self.start_time = time.time()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        if traceback is not None:
            return
        end_time = time.time()
        execution_time = end_time - self.start_time
        if self.description:
            logging.info(colored(f"Finished {self.description} in {execution_time}s", "green"))

    def get_time(self):
        return time.time() - self.start_time
