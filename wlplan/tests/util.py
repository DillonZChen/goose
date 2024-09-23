import logging

LOGGER = logging.getLogger(__name__)

def print_mat(mat: list[list], rjust: bool = True):
    if not mat:
        LOGGER.warning("Empty matrix")
        return

    max_lengths = [max(len(str(row[i])) for row in mat) for i in range(len(mat[0]))]

    for row in mat:
        ret = ""
        for i, cell in enumerate(row):
            if rjust:
                ret += str(cell).rjust(max_lengths[i]) + "  "
            else:
                ret += str(cell).ljust(max_lengths[i]) + "  "
        LOGGER.info(ret)
