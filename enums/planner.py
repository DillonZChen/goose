from util.parseable_enum import ParseableEnum


class Planner(ParseableEnum):
    NONE = "none"
    DOWNWARD = "downward"
    NUMERIC_DOWNWARD = "numeric-downward"
    POWERLIFTED = "powerlifted"
    POLICY = "policy"
    DOWNWARD_FDR = "downward-fdr"

    DOWNWARD_BLIND = "downward(blind)"
    DOWNWARD_GC = "downward(gc)"
    DOWNWARD_FF = "downward(ff)"

    POWERLIFTED_BLIND = "powerlifted(blind)"
    POWERLIFTED_GC = "powerlifted(gc)"
    POWERLIFTED_FF = "powerlifted(ff)"
    POWERLIFTED_WLNS_FF = "powerlifted(wlns-ff)"

    @staticmethod
    def requires_model(planner: "Planner") -> bool:
        return planner in {
            Planner.DOWNWARD,
            Planner.NUMERIC_DOWNWARD,
            Planner.POWERLIFTED,
            Planner.POLICY,
            Planner.DOWNWARD_FDR,
        }

    @staticmethod
    def standalone_downward_planner(planner: "Planner") -> bool:
        return planner in {Planner.DOWNWARD_BLIND, Planner.DOWNWARD_GC, Planner.DOWNWARD_FF}

    @staticmethod
    def standalone_powerlifted_planner(planner: "Planner") -> bool:
        return planner in {
            Planner.POWERLIFTED_BLIND,
            Planner.POWERLIFTED_GC,
            Planner.POWERLIFTED_FF,
            Planner.POWERLIFTED_WLNS_FF,
        }

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
            Planner.DOWNWARD_BLIND,
            Planner.DOWNWARD_GC,
            Planner.DOWNWARD_FF,
            Planner.POWERLIFTED_BLIND,
            Planner.POWERLIFTED_GC,
            Planner.POWERLIFTED_FF,
            Planner.POWERLIFTED_WLNS_FF,
        }
