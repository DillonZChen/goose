import logging
import sys


class RelativeSeconds(logging.Formatter):
    def format(self, record):
        record.relativeCreated = f"{record.relativeCreated / 1000:.4f}s"
        return super().format(record)


def init_logger():
    formatter = RelativeSeconds("[%(levelname)s t=%(relativeCreated)s] %(message)s")
    logging.basicConfig(stream=sys.stdout, level=logging.INFO)
    logging.root.handlers[0].setFormatter(formatter)
