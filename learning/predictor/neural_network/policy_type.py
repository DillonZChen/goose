from enum import Enum


class PolicyType(Enum):
    VALUE_FUNCTION = "v"
    QUALITY_FUNCTION = "q"
    ADVANTAGE_FUNCTION = "a"
    DISTRIBUTION = "d"


def get_policy_type_options():
    return [policy_type.value for policy_type in PolicyType]
