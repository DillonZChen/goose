from dataclasses import dataclass
from typing import Dict, List


@dataclass
class NumericState:
    true_facts: List[str]
    fluent_values: Dict[str, float]

    def __post_init__(self):
        tmp = self.true_facts
        self.true_facts = []
        for fact in tmp:
            if fact.startswith("="):
                continue
            self.true_facts.append(fact)
        self.true_facts = sorted(self.true_facts)
        self.fluent_values = dict(sorted(self.fluent_values.items()))

    def dump(self) -> None:
        for fact in self.true_facts:
            print(f"{fact} -> true")
        for fluent, val in self.fluent_values.items():
            print(f"{fluent} -> {val}")

    def value(self, fluent: str) -> float:
        assert fluent in self.fluent_values
        return self.fluent_values[fluent]

    def __hash__(self) -> int:
        true_facts = sorted(self.true_facts)
        fluent_values = dict(sorted(self.fluent_values.items()))
        return hash((tuple(true_facts), frozenset(fluent_values.items())))


def _transform_fact_str(toks: List[str]) -> str:
    if len(toks) == 1:
        return f"{toks[0]}()"
    else:
        assert len(toks) > 1
        return f"{toks[0]}({', '.join(toks[1:])})"
    
     
def _strip_brackets(fact: str) -> str:
    if fact.startswith("("):
        fact = fact[1:]
    if fact.endswith(")"):
        fact = fact[:-1]
    return fact


def str_to_state(str_state: str, nfd_vars=None, parse_goal=False, collect_static=False):
    if collect_static:
        assert nfd_vars is not None
    str_state = str_state.replace("\n", " ")
    str_state = str_state.replace("\t", " ")
    while "  " in str_state:
        str_state = str_state.replace("  ", " ")
    while str_state.startswith(" "):
        str_state = str_state[1:]
    while str_state.endswith(" "):
        str_state = str_state[:-1]

    chunks = []
    brackets = 0
    prev_i = 0
    for i, c in enumerate(str_state):
        if c == "(":
            brackets += 1
        elif c == ")":
            brackets -= 1
            if brackets == 0:
                chunks.append(str_state[prev_i:i + 1])
                prev_i = i + 1

    true_facts = []
    fluent_values = {}
    conditions = []
    for fact in chunks:
        fact = fact.strip()
        fact = _strip_brackets(fact)

        toks = fact.split()
        if parse_goal and (
            fact.startswith("=") or 
            fact.startswith(">=") or 
            fact.startswith("<=")
        ):  
            ## is a numeric condition
            comparator = toks[0]
            last_tok = toks[-1]
            try:
                val = float(last_tok)
                expr = " ".join(toks[1:-1])
            except ValueError:
                val = 0
                expr = " ".join(toks[1:])
            conditions.append((expr, val, comparator))
        elif not fact.startswith("="):
            ## is a fact
            assert "=" not in fact, str_state
            fact = _transform_fact_str(toks)
            if collect_static and fact in nfd_vars:
                continue
            elif not collect_static and nfd_vars is not None and fact not in nfd_vars:
                continue
            true_facts.append(fact)
        else:
            ## is a fluent
            assert fact.startswith("=")
            val = float(toks[-1])
            fluent = toks[1:-1]
            fluent = " ".join(fluent)
            fluent = _strip_brackets(fluent)
            fluent = _transform_fact_str(fluent.split())
            if collect_static and fluent in nfd_vars:
                continue
            elif not collect_static and nfd_vars is not None and fluent not in nfd_vars:
                continue
            fluent_values[fluent] = val
    if parse_goal:
        return true_facts, conditions
    elif collect_static:
        return true_facts, fluent_values
    else:
        return NumericState(true_facts, fluent_values)
