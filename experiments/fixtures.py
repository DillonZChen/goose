import os
import sys
import time
from datetime import datetime, timedelta
from itertools import product

try:
    import numpy as np
except ImportError:
    np = None

CUR_DIR = os.path.dirname(os.path.realpath(__file__))
GOOSE_DIR = os.path.normpath(f"{CUR_DIR}/../goose")


##################################################################################################
# Configurations
##################################################################################################
DOMAINS = [
    "blocksworld",
    "childsnack",
    "ferry",
    "floortile",
    "miconic",
    "rovers",
    "satellite",
    "sokoban",
    "spanner",
    "transport",
]
PROBLEMS = sorted(f"p{x}_{y:02d}" for y in range(1, 31) for x in [0, 1, 2])
MODEL_CONFIGS = {
    "features": ["wl", "iwl", "niwl", "lwl2"],
    "graph_representation": ["ilg"],
    "iterations": [1, 2, 3, 4, 5, 6, 7, 8],
    "optimisation": ["lasso", "gpr", "svr", "rank-lp", "rank-gpc", "rank-svm"],
    "feature_pruning": ["none", "i-mf"],
    "data_pruning": ["equivalent-weighted"],
    "data_generation": ["plan"],
    "facts": ["fd", "all"],
    "hash": ["mset", "set"],
}
PLAN_TIMEOUT = 60


def get_configs(description):
    return {
        "CUR_DIR": CUR_DIR,
        "GOOSE_DIR": GOOSE_DIR,
        "GOOSE_SIF": f"{GOOSE_DIR}/goose.sif",
        "TMP_DIR": get_tmp_dir(description),
        "LOG_DIR": get_log_dir(description),
        "LOCK_DIR": get_lock_dir(description),
        "MODEL_DIR": f"{CUR_DIR}/models",
        "SAS_BENCHMARK_DIR": f"{CUR_DIR}/sas-benchmarks",
        "PDDL_BENCHMARK_DIR": f"{GOOSE_DIR}/benchmarks/ipc23lt",
        "DOMAINS": DOMAINS,
        "PROBLEMS": PROBLEMS,
        "MODEL_CONFIGS": MODEL_CONFIGS,
        "JOB_SCRIPT": f"{description}.sh",
    }


def get_model_config_combinations():
    ret = []
    for config in product(*list(MODEL_CONFIGS.values())):
        c = dict(zip(MODEL_CONFIGS.keys(), config))
        if c["features"] in {"niwl", "iwl"} and c["feature_pruning"] != "none":
            continue
        ret.append(config)
    return ret


def get_tmp_dir(description):
    ret = os.path.normpath(f"{CUR_DIR}/.tmp_{description}")
    os.makedirs(ret, exist_ok=True)
    return ret


def get_lock_dir(description):
    ret = os.path.normpath(f"{CUR_DIR}/.lock_{description}")
    os.makedirs(ret, exist_ok=True)
    return ret


def get_log_dir(description):
    ret = os.path.normpath(f"{CUR_DIR}/logs_{description}")
    os.makedirs(ret, exist_ok=True)
    return ret


##################################################################################################
# Perc (if tqdm is not available) https://github.com/PaoloLRinaldi/progress_bar_python
##################################################################################################
class Perc:
    def __init__(self, vmax, verbose=3, inline=True, showbar=True, disable=False, title=None):
        if isinstance(vmax, int):
            self._vmax = vmax
            self._it = 0
        else:
            self._vmax = len(vmax)
            self._it = -1
            self._tomanage = iter(vmax)
        self._perc = -1
        self._times = [time.time()]
        self._verbose = verbose
        self._inline = inline
        self._showbar = showbar
        self._progsz = 20
        self._passedits = list()
        self._disable = disable
        self._title = title
        self._starttime = datetime.now()

    def __new__(cls, *args, **kwargs):
        return super(Perc, cls).__new__(cls)

    def tomins(self, secs):
        secs = int(round(secs))
        mins = secs // 60
        secs = secs % 60
        strhours = ""
        if mins >= 60:
            hours = mins // 60
            mins = mins % 60
            mins = "{:02d}".format(mins)
            strhours = str(hours) + ":"

        if secs < 10:
            return strhours + "{}:0{}".format(mins, secs)
        return strhours + "{}:{}".format(mins, secs)

    def next(self, it=None):
        if self._disable:
            return
        if it is not None:
            self._it = it
        self._it += 1
        current_perc = self._it * 100 // self._vmax
        if current_perc != self._perc:
            if self._inline:
                print("\r", end="")
            if self._title is not None:
                print(self._title, end=" ")
            if self._showbar:
                prog = int((self._it / self._vmax) * self._progsz)
                print("[" + "=" * prog, end="")
                if prog != self._progsz:
                    print(">" + "." * (self._progsz - prog - 1), end="")
                print("] ", end="")
            print("{}%".format(current_perc), end="")
            if self._verbose > 0:
                self._times.append(time.time())
                self._passedits.append(self._it)
                if len(self._times) > 2:
                    step = self._times[-1] - self._times[-2]
                    itspersec = (
                        self._passedits[-1] - self._passedits[-2]) / step
                    print(
                        " | %i/%i %smin/perc %.2fit/s" % (
                            self._it, self._vmax, self.tomins(step), itspersec),
                        end="",
                    )
                    if self._verbose > 1:
                        elps = self._times[-1] - self._times[0]
                        print(" | %s" % (self.tomins(elps)), end="")
                        if self._verbose > 2 and current_perc != 100 and np:
                            p = np.poly1d(
                                np.polyfit(
                                    self._passedits,
                                    self._times[1:],
                                    w=np.arange(1, len(self._times)),
                                    deg=1,
                                )
                            )
                            secs_remaining = p(self._vmax) - p(self._it)
                            print(
                                "<%s => %s"
                                % (
                                    self.tomins(secs_remaining),
                                    self.tomins(secs_remaining + elps),
                                ),
                                end="",
                            )
                            if self._verbose > 3:
                                endtime = self._starttime + \
                                    timedelta(seconds=elps + secs_remaining)
                                print(
                                    " | Started: %s - Ends at: %s"
                                    % (
                                        self._starttime.strftime("%H:%M:%S"),
                                        endtime.strftime("%H:%M:%S"),
                                    ),
                                    end="",
                                )
                                if self._verbose > 4:
                                    nxt = p(
                                        int(round((current_perc + 1) * self._vmax / 100 - 0.5) + 1)) - p(self._it)
                                    print(" | Next in %.1f/%s" %
                                          (nxt, self.tomins(nxt)), end="")
                if not self._inline:
                    print()
            if self._it == self._vmax:
                self._printdone()
            self._perc = current_perc

    def _printdone(self):
        if self._inline:
            print("\r", end="")
            sys.stdout.write("\033[2K\033[1G")
        print("Done in %s at %s" % (self.tomins(time.time() -
              self._times[0]), datetime.now().strftime("%H:%M:%S")))

    def done(self):
        if self._it != self._vmax and not self._disable:
            self._printdone()

    def __iter__(self):
        return self

    def __next__(self):
        if self._it < self._vmax:
            try:
                self.next()
            except ZeroDivisionError:
                pass
            return next(self._tomanage)
        else:
            raise StopIteration
