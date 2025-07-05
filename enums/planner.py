from util.parseable_enum import ParseableEnum


class Planner(ParseableEnum):
    NONE = None
    FD = "fd"
    NFD = "nfd"
    PWL = "pwl"
    POLICY = "policy"
