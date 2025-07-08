from util.parseable_enum import ParseableEnum


class StateRepresentation(ParseableEnum):
    DOWNWARD = "downward"
    NUMERIC_DOWNWARD = "numeric-downward"
    NO_STATICS = "no-statics"
    ALL = "all"
