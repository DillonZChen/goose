from goose.util.parseable_enum import ParseableEnum


class PolicyType(ParseableEnum):
    SEARCH = "search"

    # Predict V(s) functions
    VALUE_FUNCTION = "v"

    # Predict Q(s, a) functions; Q for quality https://en.wikipedia.org/wiki/Q-learning
    QUALITY_FUNCTION = "q"

    # Predict advantage function: A(s, a) = Q(s, a) - V(s)
    ADVANTAGE_FUNCTION = "a"

    # Predict distribution over actions: P(a | s); technically [0, 1] predictions on actions
    POLICY_FUNCTION = "p"
    POLICY_FUNCTION_X = "px"

    @staticmethod
    def is_policy_function(policy_type: "PolicyType") -> bool:
        return policy_type in {PolicyType.POLICY_FUNCTION, PolicyType.POLICY_FUNCTION_X}

    @staticmethod
    def is_not_search(policy_type: "PolicyType") -> bool:
        return policy_type != PolicyType.SEARCH
