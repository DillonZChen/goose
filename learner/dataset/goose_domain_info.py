import os
import re

GOOSE_DOMAINS = [
    "gripper",
    "spanner",
    "visitall",
    "visitsome",
    "blocks",
    "ferry",
    "sokoban",
    "n-puzzle",
]


DOMAINS_NOT_TO_TRAIN = {
    "ipc-1998-gripper-1",
    "ipc-2000-blocks",
    "ipc-2011-visit-all",
    "ipc-2014-visit-all",
}


def sorted_nicely(l):  # https://stackoverflow.com/a/2669120/13531424
    """Sort the given iterable in the way that humans expect."""
    convert = lambda text: int(text) if text.isdigit() else text
    alphanum_key = lambda key: [convert(c) for c in re.split("([0-9]+)", key)]
    return sorted(l, key=alphanum_key)


def get_domain_instance_pddl_for_domain(domain: str, split: str):
    ret = []
    dir_of_pddls = f"../dataset/goose/{domain}"
    df = f"{dir_of_pddls}/domain.pddl"
    for file in sorted_nicely(os.listdir(f"{dir_of_pddls}/{split}")):
        pf = f"{dir_of_pddls}/{split}/{file}"
        ret.append((f"goose-{domain}", df, pf))
    return ret


def get_all_goose_instance_files():
    ret = []
    for domain in sorted(GOOSE_DOMAINS):
        ret += get_domain_instance_pddl_for_domain(domain, "instances")
    # print(f"num goose train instances: {len(ret)}")
    return ret


def get_train_goose_instance_files():
    ret = []
    for domain in sorted(GOOSE_DOMAINS):
        ret += get_domain_instance_pddl_for_domain(domain, "train")
    # print(f"num goose train instances: {len(ret)}")
    return ret


def get_test_goose_instance_files():
    ret = []
    for domain in sorted(GOOSE_DOMAINS):
        ret += get_domain_instance_pddl_for_domain(domain, "test")
    # print(f"num goose test instances: {len(ret)}")
    return ret
