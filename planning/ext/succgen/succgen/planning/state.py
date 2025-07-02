from _succgen.planning import State as State
from pddl.logic.terms import Term

from succgen.planning.util import Instantiation


def pddl_terms_to_row(obj_to_i: dict[str, int], terms: list[Term]) -> Instantiation:
    return tuple(obj_to_i[t.name] for t in terms)


__all__ = ["pddl_terms_to_row", "State"]
