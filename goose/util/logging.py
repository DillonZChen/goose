import argparse
import logging
import sys
from typing import Optional

import termcolor as tc

from goose.util.parseable_enum import ParseableEnum


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
        level = record.levelname
        record.levelname = self.color(level, level)
        return super().format(record)


def init_logger(log_level=logging.INFO) -> None:
    formatter = ColoredFormatter("[%(levelname)s t=%(relativeCreated)s] %(message)s")
    logging.basicConfig(stream=sys.stdout, level=log_level)
    logging.root.handlers[0].setFormatter(formatter)


def mat_to_str(
    mat: list[list],
    rjust: Optional[list[bool]] = None,
    max_lengths: Optional[list[Optional[int]]] = None,
) -> str:
    if not mat:
        logging.warning("Empty matrix")
        return ""

    max_row_length = max(len(row) for row in mat)
    for i in range(len(mat)):
        row = list(mat[i]) + [""] * (max_row_length - len(mat[i]))
        mat[i] = row
    if max_lengths is None:
        max_lengths = [None for _ in range(len(mat[0]))]
    for i in range(len(mat[0])):
        if max_lengths[i] is None:
            max_lengths[i] = max(len(str(row[i])) + 1 for row in mat)

    ret = []
    for row in mat:
        row_ret = []
        for i, cell in enumerate(row):
            if cell == "*":
                cell = "*" * (max_lengths[i] - 1)
            if rjust is not None and rjust[i]:
                row_ret.append(str(cell).rjust(max_lengths[i]))
            else:
                row_ret.append(str(cell).ljust(max_lengths[i]))
        ret.append(" ".join(row_ret))
    return "\n".join(ret)


def log_opts(desc: str, opts: argparse.Namespace) -> None:
    coloured_mat = []
    for k, v in vars(opts).items():
        if isinstance(v, ParseableEnum):
            v = v.value
        v = tc.colored(v, "cyan")
        coloured_mat.append([k, v])
    mat_str = mat_to_str(mat=coloured_mat, max_lengths=[22, None])
    logging.info(f"{desc.upper()} OPTIONS:\n{mat_str}")
