from typing import List


def var_to_str(var):
    var_str = str(var)
    # UP removes brackets from nullary variables
    if "(" not in var_str:
        assert ")" not in var_str
        var_str += "()"
    return var_str


def var_to_predicate(var: str) -> List[str]:
    return var.split("(")[0]


def var_to_objects(var: str) -> List[str]:
    objects = var.split("(")
    if len(objects) == 1:
        return []
    objects = objects[1].replace(")", "").split(", ")
    if len(objects) == 1 and objects[0] == "":
        return []
    return objects
