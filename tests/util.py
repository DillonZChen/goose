import os
import sys
import subprocess
from subprocess import PIPE

TOL = 1e-6


def in_venv():
    return sys.prefix != sys.base_prefix


def get_output(cmd):
    print("Executing:", cmd)
    cmd = cmd.split(" ")
    p = subprocess.Popen(cmd, stdout=PIPE, stderr=PIPE)
    stdout, stderr = p.communicate()
    p.wait()
    log_output = list(str(stdout.decode("utf-8")).split("\n"))
    err_output = list(str(stderr.decode("utf-8")).split("\n"))
    return (log_output, err_output)


def log(output, file):
    log_output, err_output = output
    output = log_output + err_output
    with open(file, "w") as f:
        for line in output:
            f.write(line + "\n")
    print("log @", os.getcwd() + "/" + file)
    return


def scrape_search_log(file):
    """assumes fast downward like log files"""

    stats = {
        "first_h": -1,
        "solved": 0,
        "time": -1,
        "cost": -1,
        "expanded": -1,
        "evaluated": -1,
        "seen_colours": -1,  # for the wl methods only
        "unseen_colours": -1,
        "ratio_seen_colours": -1,
        "std": -1,
        "tried": 0,
    }

    if not os.path.exists(file):
        return stats

    with open(file, "r") as f:
        for line in f.readlines():
            line = line.replace(" state(s).", "")
            toks = line.split()
            if len(toks) == 0:
                continue

            if "Solution found." in line:
                stats["solved"] = 1
            elif "Search time:" in line:
                stats["time"] = float(toks[-1].replace("s", ""))
            elif "Plan cost:" in line:
                stats["cost"] = int(toks[-1])
            elif len(toks) >= 2 and "Expanded" == toks[-2]:
                stats["expanded"] = int(toks[-1])
            elif len(toks) >= 2 and "Evaluated" == toks[-2]:
                stats["evaluated"] = int(toks[-1])
            elif "Initial heuristic value" in line:
                stats["first_h"] = int(toks[-1])
            elif "seen/unseen colours in itr" in line:
                stats["seen_colours"] += int(toks[-2])
                stats["unseen_colours"] += int(toks[-1])
            elif "Computed std at initial state:" in line:
                stats["std"] = float(toks[-1])

    if stats["seen_colours"] != -1 and stats["unseen_colours"] != -1:
        stats["seen_colours"] += 1
        stats["unseen_colours"] += 1
        stats["ratio_seen_colours"] = stats["seen_colours"] / (
            stats["seen_colours"] + stats["unseen_colours"]
        )

    if stats["time"] > 1800:  # assume timeout is 1800
        stats["solved"] = 0
        stats["time"] = 1800

    stats["tried"] = 1

    return stats
