from util.parseable_enum import ParseableEnum


class Planner(ParseableEnum):
    NONE = "none"
    DOWNWARD = "downward"
    NUMERIC_DOWNWARD = "numeric-downward"
    POWERLIFTED = "powerlifted"
    POLICY = "policy"
    DOWNWARD_FDR = "downward-fdr"

    BLIND = "blind"
    WL = "wl"
    GC = "gc"
    FF = "ff"
    GC_WL = "gc-wl"
    FF_WL = "ff-wl"
    FF_GC = "ff-gc"

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
        return planner in {
            Planner.BLIND,
            Planner.WL,
            Planner.GC,
            Planner.FF,
            Planner.GC_WL,
            Planner.FF_WL,
            Planner.FF_GC,
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
            Planner.BLIND,
            Planner.WL,
            Planner.GC,
            Planner.FF,
            Planner.GC_WL,
            Planner.FF_WL,
            Planner.FF_GC,
        }
