from goose.util.parseable_enum import ParseableEnum


class Planner(ParseableEnum):
    NONE = "none"
    DOWNWARD = "downward"
    DOWNWARD_FDR = "downward-fdr"
    NUMERIC_DOWNWARD = "numeric-downward"
    POWERLIFTED = "powerlifted"
    POLICY = "policy"

    @staticmethod
    def supports_fdr(planner: "Planner") -> bool:
        return planner in {
            Planner.DOWNWARD_FDR,
        }

    @staticmethod
    def supports_pddl(planner: "Planner") -> bool:
        return planner in {
            Planner.DOWNWARD,
            Planner.NUMERIC_DOWNWARD,
            Planner.POWERLIFTED,
            Planner.POLICY,
        }
