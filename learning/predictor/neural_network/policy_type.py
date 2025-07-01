from enum import Enum


class PolicyType(Enum):
    # Predict V(s) functions
    VALUE_FUNCTION = "v"
    # Predict Q(s, a) functions; Q for quality https://en.wikipedia.org/wiki/Q-learning
    QUALITY_FUNCTION = "q"
    # Predict advantage function: A(s, a) = Q(s, a) - V(s)
    ADVANTAGE_FUNCTION = "a"
    # Predict distribution over actions: P(a | s)
    DISTRIBUTION = "d"


def get_policy_type_options():
    return [policy_type.value for policy_type in PolicyType]
