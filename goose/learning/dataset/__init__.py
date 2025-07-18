import argparse
import os

from wlplan.planning import Domain, parse_domain


def get_domain_file_from_opts(opts: argparse.Namespace) -> str:
    return os.path.normpath(f"{opts.domain_directory}/domain.pddl")


def get_training_dir_from_opts(opts: argparse.Namespace) -> str:
    return os.path.normpath(f"{opts.domain_directory}/training")


def get_training_plans_dir_from_opts(opts: argparse.Namespace) -> str:
    return os.path.normpath(f"{opts.domain_directory}/training_plans")


def get_domain_from_opts(opts: argparse.Namespace) -> Domain:
    return parse_domain(get_domain_file_from_opts(opts))
