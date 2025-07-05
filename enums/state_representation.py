from util.parseable_enum import ParseableEnum


class StateRepresentation(ParseableEnum):
    FD = "fd"
    NFD = "nfd"
    NO_STATIC = "nostatic"
    ALL = "all"
