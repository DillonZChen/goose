#!/usr/bin/env python3
import argparse
import csv
import os
import subprocess
import sys
import time
from datetime import datetime, timedelta
from itertools import product

try:
    import numpy as np
except ImportError:
    np = None

CUR_DIR = os.path.dirname(os.path.realpath(__file__))
ROOT_DIR = os.path.normpath(f"{CUR_DIR}/../..")
DOMAINS = [row[0] for row in csv.reader(open(f"{ROOT_DIR}/domains.csv"))]
PLANNERS = [row[0] for row in csv.reader(open(f"{ROOT_DIR}/planners.csv"))]
LEARNERS = set(row[0] for row in csv.reader(open(f"{ROOT_DIR}/learners.csv")))
DIDP_DOMAINS = set([row[0] for row in csv.reader(open(f"{ROOT_DIR}/didp_domains.csv"))])
LRNN_DOMAINS = set([row[0] for row in csv.reader(open(f"{ROOT_DIR}/lrnn_domains.csv"))])
INCOMPLETE_DOMAINS = set([row[0] for row in csv.reader(open(f"{ROOT_DIR}/incomplete_domains.csv"))])
INCOMPLETE_PLANNERS = set([row[0] for row in csv.reader(open(f"{ROOT_DIR}/incomplete_planners.csv"))])
NO_AXIOM_PLANNERS = set([row[0] for row in csv.reader(open(f"{ROOT_DIR}/no_axiom_planners.csv"))])

PROBLEMS = [f"{x}_{y:02d}" for y in range(1, 31) for x in [0, 1, 2]]
GP_REPEATS = range(1)

if os.path.exists("/pfcalcul/work/dchen"):
    CLUSTER_NAME = "pfcalcul"
    CLUSTER_TYPE = "slurm"
elif os.path.exists("/scratch/cd85/dc6693"):
    CLUSTER_NAME = "gadi"
    CLUSTER_TYPE = "pbs"
else:
    raise RuntimeError("Not on a compute cluster.")

## paths
TMP_DIR = os.path.normpath(f"{CUR_DIR}/../_tmp_plan")
LCK_DIR = os.path.normpath(f"{CUR_DIR}/../_lck_plan")
LOG_DIR = os.path.normpath(f"{CUR_DIR}/../_log_plan/{CLUSTER_NAME}")
os.makedirs(LOG_DIR, exist_ok=True)
JOB_SCRIPT = f"{CUR_DIR}/job_plan.sh"
assert os.path.exists(JOB_SCRIPT), JOB_SCRIPT

""" Main loop """


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("submissions", type=int)
    parser.add_argument("-l", "--remove_locks", action="store_true")
    parser.add_argument("-f", "--force", action="store_true")
    parser.add_argument("-w", "--which", action="store_true", help="see which jobs to go")
    parser.add_argument(
        "-rt",
        "--remove_terminated",
        action="store_true",
        help="see which jobs are manually terminated and remove them",
    )
    parser.add_argument(
        "-rf",
        "--remove_failed",
        action="store_true",
        help="remove jobs with container failure",
    )
    parser.add_argument(
        "-rd",
        "--remove_domain",
        type=str,
        choices=DOMAINS,
        help="remove all logs and locks for a domain",
    )
    parser.add_argument(
        "-rp",
        "--remove_planner",
        type=str,
        choices=PLANNERS,
        help="remove all logs and locks for a planner",
    )
    parser.add_argument(
        "-d",
        "--domain",
        type=str,
        choices=DOMAINS,
        help="submit jobs only from specified domain",
    )
    parser.add_argument(
        "-p",
        "--planner",
        type=str,
        choices=PLANNERS,
        help="submit jobs only from specified planner",
    )
    args = parser.parse_args()

    submissions = args.submissions
    skipped_from_missing_model = 0
    skipped_from_lock = 0
    skipped_from_log = 0
    to_go = 0

    if args.remove_locks:
        os.system(f"rm -rf {LCK_DIR}")
        print("Locks removed. Exiting.")
        return

    done_planners = {p: 0 for p in PLANNERS}
    done_domains = {d: 0 for d in DOMAINS}

    submitted = 0

    for domain, problem, planner, repeat in Perc(
        list(product(DOMAINS, PROBLEMS, PLANNERS, GP_REPEATS))
    ):  
        # Check which configs to skip

        # now have fd-translator to didp, so should work for any domain
        # if planner.startswith("didp-") and domain not in DIDP_DOMAINS:
            # continue

        if planner == "lrnn" and domain not in LRNN_DOMAINS:
            continue

        if planner.startswith("vanir"):
            i = planner[len("vanir"):]
            model_path = f"{ROOT_DIR}/planning/vanir/models/{domain}_{i}/output/sketch_minimized_{i}.txt"
            if not os.path.exists(model_path):
                continue
        
        if planner in INCOMPLETE_PLANNERS and domain in INCOMPLETE_DOMAINS:
            continue

        if "goose" in planner or "lrnn" in planner:
            learner_name = planner.replace("fd-", "")
            learner_name = learner_name.replace("gbfs-", "")
            learner_name = learner_name.replace("gp-", "")
            model_path = f"{ROOT_DIR}/experiments/_models/{domain}_{learner_name}_{repeat}.model"
            if not os.path.exists(model_path):
                skipped_from_missing_model += 1
                continue
        
        # Submit jobs
        job_description = f"{planner}_{domain}_{problem}"
        log_file = f"{LOG_DIR}/{planner}/{domain}_{problem}"
        lck_file = f"{LCK_DIR}/{planner}/{domain}_{problem}"

        if planner.startswith("gp-"):
            job_description += f"_{repeat}"
            log_file += f"_{repeat}"
            lck_file += f"_{repeat}"
        elif repeat != 0:
            continue

        log_file += ".log"
        lck_file += ".lck"
        os.makedirs(os.path.dirname(log_file), exist_ok=True)
        os.makedirs(os.path.dirname(lck_file), exist_ok=True)

        if args.domain and not domain == args.domain:
            continue

        if args.planner and not planner == args.planner:
            continue

        if args.remove_terminated:
            if not os.path.exists(log_file):
                continue
            with open(log_file) as f:
                content = f.read()
            if "Exit Status:        0" in content:
                continue
            if "Job failed due to exceeding walltime" in content:
                continue
            if "PBS: job killed: walltime" in content:
                continue
            assert "SIGTERM Termination" in content, log_file
            os.remove(log_file)
            print(f"Removed {job_description}")
            continue
        
        if args.remove_failed:
            if not os.path.exists(log_file):
                continue
            with open(log_file) as f:
                content = f.read()
            if "FATAL:   container creation failed: mount" in content:
                os.remove(log_file)
                print(f"Removed {job_description}")
            continue

        if args.remove_domain:
            if not domain == args.remove_domain:
                continue
            removed = False
            if os.path.exists(log_file):
                os.remove(log_file)
                removed = True
            if os.path.exists(lck_file):
                os.remove(lck_file)
                removed = True
            if removed:
                print(f"Removed {job_description}")
            continue

        if args.remove_planner:
            if not planner == args.remove_planner:
                continue
            removed = False
            if os.path.exists(log_file):
                os.remove(log_file)
                removed = True
            if os.path.exists(lck_file):
                os.remove(lck_file)
                removed = True
            if removed:
                print(f"Removed {job_description}")
            continue

        if os.path.exists(lck_file) and not args.force:
            skipped_from_lock += 1
            continue

        if os.path.exists(log_file) and not args.force:
            skipped_from_log += 1
            done_planners[planner] += 1
            done_domains[domain] += 1
            continue

        if args.which:
            print(f"{job_description=}")
            to_go += 1
            continue

        if submitted >= submissions:
            to_go += 1
            continue

        # domain_pddl = f"{ROOT_DIR}/benchmarks/{domain}-axioms/domain.pddl"
        # problem_pddl = f"{ROOT_DIR}/benchmarks/{domain}-axioms/testing/p{problem}.pddl"
        script = f"{ROOT_DIR}/planning/plan.py"
        tmp_dir = f"{TMP_DIR}/{job_description}"

        # domain_pddl = os.path.normpath(domain_pddl)
        # problem_pddl = os.path.normpath(problem_pddl)
        script = os.path.normpath(script)
        tmp_dir = os.path.normpath(tmp_dir)

        cmd = f"python3 {script} {domain} {problem} {planner} --seed {repeat}"
        if planner in NO_AXIOM_PLANNERS:
            cmd += " -na"
            cmd = cmd.replace("-noaxioms", "")

        job_vars = ",".join(
            [
                f"CMD={cmd}",
                f"TMP_DIR={tmp_dir}",
            ]
        )

        if CLUSTER_TYPE == "slurm":
            job_cmd = [
                "sbatch",
                f"--job-name=P_{job_description}",
                f"--output={log_file}",
                f"--export={job_vars}",
                JOB_SCRIPT,
            ]
        elif CLUSTER_TYPE == "pbs":
            job_cmd = [
                "qsub",
                "-N",
                f"P_{job_description}",
                "-o",
                log_file,
                "-j",
                "oe",
                "-v",
                job_vars,
                JOB_SCRIPT,
            ]

        with open(lck_file, "w") as f:
            f.write("")
        p = subprocess.Popen(job_cmd)
        p.wait()

        print(f" log: {log_file}")
        submitted += 1

    if args.remove_terminated or args.remove_failed or args.remove_domain or args.remove_planner:
        return

    print("*" * 80)
    for d in DOMAINS:
        print(f"{d}: {done_domains[d]}")
    print("*" * 80)
    for p in PLANNERS:
        print(f"{p}: {done_planners[p]}")
    print("*" * 80)
    print(f"{submitted=}")
    print(f"{skipped_from_missing_model=}")
    print(f"{skipped_from_lock=}")
    print(f"{skipped_from_log=}")
    print(f"{to_go=}")
    print("*" * 80)


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
                    itspersec = (self._passedits[-1] - self._passedits[-2]) / step
                    print(
                        " | %i/%i %smin/perc %.2fit/s"
                        % (self._it, self._vmax, self.tomins(step), itspersec),
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
                                % (self.tomins(secs_remaining), self.tomins(secs_remaining + elps)),
                                end="",
                            )
                            if self._verbose > 3:
                                endtime = self._starttime + timedelta(seconds=elps + secs_remaining)
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
                                        int(round((current_perc + 1) * self._vmax / 100 - 0.5) + 1)
                                    ) - p(self._it)
                                    print(" | Next in %.1f/%s" % (nxt, self.tomins(nxt)), end="")
                if not self._inline:
                    print()
            if self._it == self._vmax:
                self._printdone()
            self._perc = current_perc

    def _printdone(self):
        if self._inline:
            print("\r", end="")
            sys.stdout.write("\033[2K\033[1G")
        print(
            "Done in %s at %s"
            % (self.tomins(time.time() - self._times[0]), datetime.now().strftime("%H:%M:%S"))
        )

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


if __name__ == "__main__":
    main()
