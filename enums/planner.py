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
    DOWNWARD_ADD = "downward(add)"
    DOWNWARD_FF = "downward(ff)"
    DOWNWARD_QBWLGC = "downward(qbwlgc)"
    DOWNWARD_QBWLADD = "downward(qbwladd)"
    DOWNWARD_QBWLFF = "downward(qbwlff)"
    DOWNWARD_QBPNGC = "downward(qbpngc)"
    DOWNWARD_QBPNADD = "downward(qbpnadd)"
    DOWNWARD_QBPNFF = "downward(qbpnff)"
    DOWNWARD_QBPNWLGC = "downward(qbpnwlgc)"
    DOWNWARD_QBPNWLADD = "downward(qbpnwladd)"
    DOWNWARD_QBPNWLFF = "downward(qbpnwlff)"

    POWERLIFTED_BLIND = "powerlifted(blind)"
    POWERLIFTED_GC = "powerlifted(gc)"
    POWERLIFTED_ADD = "powerlifted(add)"
    POWERLIFTED_FF = "powerlifted(ff)"
    POWERLIFTED_QBWLGC = "powerlifted(qbwlgc)"
    POWERLIFTED_QBWLADD = "powerlifted(qbwladd)"
    POWERLIFTED_QBWLFF = "powerlifted(qbwlff)"
    POWERLIFTED_QBPNGC = "powerlifted(qbpngc)"
    POWERLIFTED_QBPNADD = "powerlifted(qbpnadd)"
    POWERLIFTED_QBPNFF = "powerlifted(qbpnff)"
    POWERLIFTED_QBPNWLGC = "powerlifted(qbpnwlgc)"
    POWERLIFTED_QBPNWLADD = "powerlifted(qbpnwladd)"
    POWERLIFTED_QBPNWLFF = "powerlifted(qbpnwlff)"

    POWERLIFTED_DQS_QBWLGC = "powerlifted(dqs-qbwlgc)"
    POWERLIFTED_DQS_QBWLADD = "powerlifted(dqs-qbwladd)"
    POWERLIFTED_DQS_QBWLFF = "powerlifted(dqs-qbwlff)"
    POWERLIFTED_DQS_QBPNGC = "powerlifted(dqs-qbpngc)"
    POWERLIFTED_DQS_QBPNADD = "powerlifted(dqs-qbpnadd)"
    POWERLIFTED_DQS_QBPNFF = "powerlifted(dqs-qbpnff)"
    POWERLIFTED_ALT_BFWS_FF = "powerlifted(alt-bfws-ff)"

    @staticmethod
    def standalone_downward_planners(values: bool = True) -> set["Planner"]:
        ret = {
            Planner.DOWNWARD_BLIND,
            Planner.DOWNWARD_GC,
            Planner.DOWNWARD_ADD,
            Planner.DOWNWARD_FF,
            Planner.DOWNWARD_QBWLGC,
            Planner.DOWNWARD_QBWLADD,
            Planner.DOWNWARD_QBWLFF,
            Planner.DOWNWARD_QBPNGC,
            Planner.DOWNWARD_QBPNADD,
            Planner.DOWNWARD_QBPNFF,
            Planner.DOWNWARD_QBPNWLGC,
            Planner.DOWNWARD_QBPNWLADD,
            Planner.DOWNWARD_QBPNWLFF,
        }
        if values:
            ret = {planner.value for planner in ret}
        return ret

    @staticmethod
    def standalone_powerlifted_planners(values: bool = True) -> set["Planner"]:
        ret = {
            Planner.POWERLIFTED_BLIND,
            Planner.POWERLIFTED_GC,
            Planner.POWERLIFTED_ADD,
            Planner.POWERLIFTED_FF,
            Planner.POWERLIFTED_QBWLGC,
            Planner.POWERLIFTED_QBWLADD,
            Planner.POWERLIFTED_QBWLFF,
            Planner.POWERLIFTED_QBPNGC,
            Planner.POWERLIFTED_QBPNADD,
            Planner.POWERLIFTED_QBPNFF,
            Planner.POWERLIFTED_QBPNWLGC,
            Planner.POWERLIFTED_QBPNWLADD,
            Planner.POWERLIFTED_QBPNWLFF,
            Planner.POWERLIFTED_DQS_QBWLGC,
            Planner.POWERLIFTED_DQS_QBWLADD,
            Planner.POWERLIFTED_DQS_QBWLFF,
            Planner.POWERLIFTED_DQS_QBPNGC,
            Planner.POWERLIFTED_DQS_QBPNADD,
            Planner.POWERLIFTED_DQS_QBPNFF,
            Planner.POWERLIFTED_ALT_BFWS_FF,
        }
        if values:
            ret = {planner.value for planner in ret}
        return ret

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
        return planner in Planner.standalone_downward_planners(values=False)

    @staticmethod
    def standalone_powerlifted_planner(planner: "Planner") -> bool:
        return planner in Planner.standalone_powerlifted_planners(values=False)

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
        } | Planner.standalone_downward_planners(values=False) | Planner.standalone_powerlifted_planners(values=False)
