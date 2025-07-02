import argparse
import logging
import os

import termcolor as tc
from succgen.util.logging import mat_to_str

from planning.util import is_domain_numeric


def get_planning_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument("domain_pddl", type=str)
    parser.add_argument("problem_pddl", type=str)
    parser.add_argument("model_path", type=str)
    parser.add_argument("-t", "--timeout", type=int, default=1800)
    parser.add_argument("-p", "--planner", type=str, default="fd", choices=["pwl", "fd", "nfd", "policy"])
    parser.add_argument("-o", "--plan_file", type=str, default="plan.plan")
    parser.add_argument("--intermediate-file", type=str, default="intermediate.tmp")
    return parser


def parse_planning_opts():
    parser = get_planning_parser()
    opts = parser.parse_args()

    # Check options
    domain_pddl = opts.domain_pddl
    if domain_pddl == "fdr":
        assert opts.planner == "fd", "FDR inputs are only supported with Fast Downward"
    else:
        assert os.path.exists(domain_pddl), domain_pddl
        if is_domain_numeric(domain_pddl) and opts.planner != "nfd":
            logging.info("Domain is numeric so switching planner to nfd.")
            opts.planner = "nfd"

    # Log parsed options
    logging.info(f"Processed options:\n{mat_to_str([[k, tc.colored(v, 'cyan')] for k, v in vars(opts).items()])}")

    return opts
