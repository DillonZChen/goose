import os

HGN_DOMAINS = {
    "blocks",
    "ferry",
    "gripper",
    "hanoi",
    "n-puzzle",
    "sokoban",
    "spanner",
    "visitall",
    "visitsome",
}

DOMAINS_NOT_TO_TRAIN = {
  "ipc-1998-gripper-1",
  "ipc-2000-blocks", 
  "ipc-2011-visit-all",
  "ipc-2014-visit-all",
}


def get_domain_instance_pddl_for_domain(domain: str, split: str):
  # assert domain in HTG_DOMAINS
  ret = []
  dir_of_pddls = f"../hgn-benchmarks/{domain}"
  df = f"{dir_of_pddls}/domain.pddl"
  for file in sorted(os.listdir(f'{dir_of_pddls}/{split}')):
    pf = f"{dir_of_pddls}/{split}/{file}"
    ret.append((f'hgn-{domain}', df, pf))
  return ret


def get_all_hgn_instance_files():
    ret = []
    for domain in sorted(HGN_DOMAINS):
        ret += get_domain_instance_pddl_for_domain(domain, "instances")
    # print(f"num hgn train instances: {len(ret)}")
    return ret


def get_train_hgn_instance_files():
    ret = []
    for domain in sorted(HGN_DOMAINS):
        ret += get_domain_instance_pddl_for_domain(domain, "train")
    # print(f"num hgn train instances: {len(ret)}")
    return ret


def get_test_hgn_instance_files():
    ret = []
    for domain in sorted(HGN_DOMAINS):
        ret += get_domain_instance_pddl_for_domain(domain, "test")
    # print(f"num hgn test instances: {len(ret)}")
    return ret

# get_train_hgn_instance_files()
# get_test_hgn_instance_files()
