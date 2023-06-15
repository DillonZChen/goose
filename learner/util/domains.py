import os
from util.goose_domain_info import *
from util.htg_domain_info import *
from util.ipc_domain_info import *

def get_problem_name(problem_file: str):
  return os.path.basename(problem_file).replace(".pddl", "")


def get_domain_name(domain_file: str):
  if "ipc-benchmarks" in domain_file:
    domain_file = domain_file.replace("/domains/", '-')
    for domain in IPC_DOMAINS:
      if domain in domain_file:
        return domain
    raise Exception(f"{domain_file} does not have an associated IPC domain")
  if "hgn-benchmarks" in domain_file:
    for domain in GOOSE_DOMAINS:
      if domain in domain_file:
        return domain
    return "hgn-domain"
    # raise Exception(f"{domain_file} does not have an associated HGN domain")
  if "htg-benchmarks" in domain_file:
    for domain in HTG_DOMAINS:
      if domain in domain_file:
        return domain
    raise Exception(f"{domain_file} does not have an associated HTG domain")
  raise Exception(f"{domain_file} does not have an associated domain")