from goose.util.parseable_enum import ParseableEnum


class Planner(ParseableEnum):
    NONE = "none"
    DOWNWARD = "downward"
    SCORPION = "scorpion"
    LAMA = "lama"
    LAMA_F = "lama-first"
    NOLAN = "nolan"
    NOLAN_G = "nolan-goose"
    NUMERIC_DOWNWARD = "numeric-downward"
    POWERLIFTED = "powerlifted"
    POLICY = "policy"

    @staticmethod
    def is_downward_alias(planner: "Planner") -> bool:
        return planner in {
            Planner.SCORPION,
            Planner.LAMA,
            Planner.LAMA_F,
            Planner.NOLAN,
            Planner.NOLAN_G,
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
