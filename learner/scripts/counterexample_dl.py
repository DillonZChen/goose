from typing import List


class Fact:
    def __init__(self, pred: str, args: List[str]) -> None:
        self.pred = pred
        self.args = args

    def __hash__(self) -> bool:
        return hash((self.pred, tuple(self.args)))

    def __eq__(self, other) -> bool:
        return self.pred == other.pred and self.args == other.args

    def __repr__(self) -> str:
        return self.pred + "(" + ",".join(self.args) + ")"

    def __lt__(self, other) -> bool:
        return repr(self) < repr(other)


class Problem:
    def __init__(
        self, name: str, props: List[Fact], goals: List[Fact]
    ) -> None:
        self.name = name
        self.props = props
        self.goals = goals

    def __eq__(self, other) -> bool:
        return sorted(self.props) == sorted(other.props) and sorted(
            self.goals
        ) == sorted(other.goals)

    def __repr__(self) -> str:
        ret = f"{self.name}\n"
        ret += "  props:\n"
        for prop in sorted(self.props):
            ret += f"    {prop}\n"
        ret += "  goals:\n"
        for prop in sorted(self.goals):
            ret += f"    {prop}\n"
        return ret


def split_fact_3_to_2s(fact: Fact):
    pred = fact.pred
    args = fact.args

    assert len(args) == 3

    pred12 = pred + "12"
    pred13 = pred + "13"
    pred23 = pred + "23"

    return [
        Fact(pred12, [args[0], args[1]]),
        Fact(pred13, [args[0], args[2]]),
        Fact(pred23, [args[1], args[2]]),
    ]


def split_prob_3_to_2(prob: Problem):
    new_props = []
    new_goals = []
    for prop in prob.props:
        new_props += split_fact_3_to_2s(prop)
    for prop in prob.goals:
        new_goals += split_fact_3_to_2s(prop)
    return Problem(
        f"split_{prob.name}", list(set(new_props)), list(set(new_goals))
    )


P1 = Problem(
    name="P1",
    props=[
        Fact("P", ["a", "b", "a"]),
        Fact("P", ["c", "b", "c"]),
        Fact("P", ["a", "d", "c"]),
        Fact("P", ["c", "d", "a"]),
    ],
    goals=[
        Fact("P", ["a", "b", "c"]),
    ],
)

P2 = Problem(
    name="P2",
    props=[
        Fact("P", ["a", "b", "c"]),
        Fact("P", ["c", "b", "a"]),
        Fact("P", ["a", "d", "a"]),
        Fact("P", ["c", "d", "c"]),
    ],
    goals=[
        Fact("P", ["a", "b", "c"]),
    ],
)


print(split_prob_3_to_2(P1))
print(split_prob_3_to_2(P2))

print(split_prob_3_to_2(P1) == split_prob_3_to_2(P2))
