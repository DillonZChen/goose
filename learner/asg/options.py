# -*- coding: utf-8 -*-

import argparse
import sys


def parse_args():
    argparser = argparse.ArgumentParser()
    argparser.add_argument(
        "domain", help="path to domain pddl file")
    argparser.add_argument(
        "task", help="path to task pddl file")
    argparser.add_argument(
        "--relaxed", dest="generate_relaxed_task", action="store_true",
        help="output relaxed task (no delete effects)")
    argparser.add_argument(
        "--full-encoding",
        dest="use_partial_encoding", action="store_false",
        help="By default we represent facts that occur in multiple "
        "mutex groups only in one variable. Using this parameter adds "
        "these facts to multiple variables. This can make the meaning "
        "of the variables clearer, but increases the number of facts.")
    argparser.add_argument(
        "--invariant-generation-max-candidates", default=100000, type=int,
        help="max number of candidates for invariant generation "
        "(default: %(default)d). Set to 0 to disable invariant "
        "generation and obtain only binary variables. The limit is "
        "needed for grounded input files that would otherwise produce "
        "too many candidates.")
    argparser.add_argument(
        "--sas-file", default="output.sas",
        help="path to the SAS output file (default: %(default)s)")
    argparser.add_argument(
        "--invariant-generation-max-time", default=300, type=int,
        help="max time for invariant generation (default: %(default)ds)")
    argparser.add_argument(
        "--add-implied-preconditions", action="store_true",
        help="infer additional preconditions. This setting can cause a "
        "severe performance penalty due to weaker relevance analysis "
        "(see issue7).")
    argparser.add_argument(
        "--keep-unreachable-facts",
        dest="filter_unreachable_facts", action="store_false",
        help="keep facts that can't be reached from the initial state")
    argparser.add_argument(
        "--skip-variable-reordering",
        dest="reorder_variables", action="store_false",
        help="do not reorder variables based on the causal graph. Do not use "
        "this option with the causal graph heuristic!")
    argparser.add_argument(
        "--keep-unimportant-variables",
        dest="filter_unimportant_vars", action="store_false",
        help="keep variables that do not influence the goal in the causal graph")
    argparser.add_argument(
        "--dump-task", action="store_true",
        help="dump human-readable SAS+ representation of the task")

    #### Options related to symmetries
    argparser.add_argument(
        "--compute-symmetries", action="store_true",
        help="compute symmetries on the normalized taks using bliss, dump "
        "statistics")
    argparser.add_argument(
        "--only-object-symmetries", action="store_true",
        help="Only allow objects to be permuted, but not "
        "predicates or functions. (Set option --compute-symmetries)")
    argparser.add_argument(
        "--do-not-stabilize-initial-state", action="store_true",
        help="If true, only those atoms in the initial state mentioning "
        "static predicates are added. (Set option --compute-symmetries)")
    argparser.add_argument(
        "--do-not-stabilize-goal", action="store_true",
        help="If true, the goal is ignored in the symmetry computation. "
        "(Set option --compute-symmetries)")
    argparser.add_argument(
        "--bliss-time-limit", default=300, type=int,
        help="Max time for bliss to search for automorphisms. (Set option "
        "--compute-symmetries)")
    argparser.add_argument(
        "--write-group-generators", action="store_true",
        help="If true, write the group generators to the file 'generators.py'. "
        "Each line contains a list of integers in the Python list format. "
        "The list is to be read as a permutation of the integers 0 to length "
        "of the list. (Set option --compute-symmetries)")
    argparser.add_argument(
        "--write-dot-graph", action="store_true",
        help="If true, write the symmetry graph in dot format to the file "
        "out.dot. (Set option --compute-symmetries)")
    argparser.add_argument(
        "--stop-after-computing-symmetries", action="store_true",
        help="If true, stop after computing symmetries. (Set option "
        "--compute-symmetries)")
    return argparser.parse_args()


def copy_args_to_module(args):
    module_dict = sys.modules[__name__].__dict__
    for key, value in vars(args).items():
        module_dict[key] = value


def setup():
    args = parse_args()
    copy_args_to_module(args)


setup()
