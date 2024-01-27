from typing import List

from .pddl_types import TypedObject


class Predicate:
    def __init__(self, name: str, arguments: List[TypedObject]) -> None:
        self.name = name
        self.arguments = arguments

    def __str__(self):
        return "%s(%s)" % (self.name, ", ".join(map(str, self.arguments)))

    def __lt__(self, other):
        return self.name < other.name

    def get_arity(self):
        return len(self.arguments)
