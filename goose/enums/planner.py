from goose.util.parseable_enum import ParseableEnum


class Planner(ParseableEnum):
    NONE = "none"
    DOWNWARD = "downward"
    SCORPION = "scorpion"
    LAMA = "lama"
    NOLAN = "nolan"
    NUMERIC_DOWNWARD = "numeric-downward"
    POWERLIFTED = "powerlifted"
    POLICY = "policy"

    @staticmethod
    def is_downward_alias(planner: "Planner") -> bool:
        return planner in {
            Planner.SCORPION,
            Planner.LAMA,
            Planner.NOLAN,
        }

    @staticmethod
    def supports_fdr(planner: "Planner") -> bool:
        return Planner.is_downward_alias(planner) or planner in {Planner.DOWNWARD}

    @staticmethod
    def supports_pddl(planner: "Planner") -> bool:
        return Planner.is_downward_alias(planner) or planner in {
            Planner.DOWNWARD,
            Planner.NUMERIC_DOWNWARD,
            Planner.POWERLIFTED,
            Planner.POLICY,
        }
