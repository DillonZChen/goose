import logging
import sys

import termcolor as tc


class RelativeSeconds(logging.Formatter):
    def format(self, record):
        record.relativeCreated = f"{record.relativeCreated / 1000:.4f}s"
        return super().format(record)


class ColoredFormatter(RelativeSeconds):
    def color(self, level: str, content: str):
        match level:
            case "DEBUG":
                color = "blue"
            case "INFO":
                color = "green"
            case "WARNING":
                color = "yellow"
            case "ERROR":
                color = "red"
            case "CRITICAL":
                color = "red"
            case _:
                color = "white"
        return tc.colored(content, color=color, attrs=["bold"])

    def format(self, record):
        # make the log level colored with colour only
        level = record.levelname
        record.levelname = self.color(level, level)
        return super().format(record)


def init_logger(log_level=logging.INFO):
    formatter = ColoredFormatter("[%(levelname)s t=%(relativeCreated)s] %(message)s")
    logging.basicConfig(stream=sys.stdout, level=log_level)
    logging.root.handlers[0].setFormatter(formatter)


def mat_to_str(mat: list[list], rjust: list[bool] = None) -> str:
    if not mat:
        logging.warning("Empty matrix")
        return ""

    mat = [[str(cell) for cell in row] for row in mat]
    max_row_length = max(len(row) for row in mat)
    for i in range(len(mat)):
        row = list(mat[i]) + [""] * (max_row_length - len(mat[i]))
        mat[i] = row
    max_lengths = [max(len(str(row[i])) + 2 for row in mat) for i in range(len(mat[0]))]

    ret = []
    for row in mat:
        row_ret = []
        for i, cell in enumerate(row):
            if str(cell).startswith("*"):
                cell = "-" * (max_lengths[i] - 1)
            if rjust is not None and rjust[i]:
                row_ret.append(str(cell).rjust(max_lengths[i]))
            else:
                row_ret.append(str(cell).ljust(max_lengths[i]))
        ret.append(" ".join(row_ret))
    return "\n".join(ret)
