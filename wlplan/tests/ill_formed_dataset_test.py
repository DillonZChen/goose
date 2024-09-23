from wlplan.data import Dataset, ProblemStates
from wlplan.planning import Atom, Domain, Predicate, Problem

## domain
on = Predicate("on", 2)
on_table = Predicate("on-table", 1)
clear = Predicate("clear", 1)
holding = Predicate("holding", 1)
arm_empty = Predicate("arm-empty", 0)
predicates = [
    on,
    on_table,
    clear,
    holding,
    arm_empty,
]

lime = Predicate("lime", 5)
on_bad = Predicate("on", 3)  # do smth about dupe pred with different arity?
bad_predicates = [
    lime,
    on_bad,
]

blocksworld_domain = Domain(
    name="blocksworld",
    predicates=predicates,
    constant_objects=["dummy_constant_block"],
)

not_blocksworld_domain = Domain(
    name="not_blocksworld",
    predicates=predicates + bad_predicates,
    constant_objects=[],
)

## problem
objects = ["a", "b", "c", "d", "e", "f", "g"]
bad_objects = ["lime", "bad", "a;owiej"]
# https://www.sciencedirect.com/science/article/pii/S0004370200000795 Fig. 1
# a f
# e d
# b c g
positive_goals = [
    Atom(clear, ["a"]),
    Atom(on, ["a", "e"]),
    Atom(on, ["e", "b"]),
    Atom(on_table, ["b"]),
    Atom(clear, ["f"]),
    Atom(on, ["f", "d"]),
    Atom(on, ["d", "c"]),
    Atom(on_table, ["c"]),
    Atom(clear, ["g"]),
    Atom(on_table, ["g"]),
]
negative_goals = []

print(f"{positive_goals=}")
print(f"{negative_goals=}")


def test_domain_mismatch():
    correct_exception_caught = False
    try:
        problem_good = Problem(blocksworld_domain, objects, positive_goals, negative_goals)
        problem_bad = Problem(not_blocksworld_domain, objects, positive_goals, negative_goals)
        data = [
            ProblemStates(problem_good, []),
            ProblemStates(problem_bad, []),
        ]
        dataset = Dataset(blocksworld_domain, data)
        print("no exception caught")
    except RuntimeError as e:
        correct_exception_caught = str(e).startswith("Domain mismatch")
        print(f"exception caught: {e}")
    assert correct_exception_caught


def test_bad_predicate_in_goal():
    try:
        negative_goals = [
            Atom(lime, ["a", "b", "c", "d", "e"]),
        ]
        problem = Problem(blocksworld_domain, objects, positive_goals, negative_goals)
        data = [
            ProblemStates(problem, []),
        ]
        dataset = Dataset(blocksworld_domain, data)
        print("no exception caught")
    except RuntimeError as e:
        correct_exception_caught = str(e).startswith("Unknown predicate")
        print(f"exception caught: {e}")
    assert correct_exception_caught


def test_bad_object_in_goal():
    try:
        negative_goals = [
            Atom(on, ["asdfasdfa", "basdfasdf"]),
        ]
        problem = Problem(blocksworld_domain, objects, positive_goals, negative_goals)
        data = [
            ProblemStates(problem, []),
        ]
        dataset = Dataset(blocksworld_domain, data)
        print("no exception caught")
    except RuntimeError as e:
        correct_exception_caught = str(e).startswith("Unknown object")
        print(f"exception caught: {e}")
    assert correct_exception_caught


def test_bad_arity_in_goal():
    try:
        negative_goals = [
            Atom(on, ["a", "b", "c"]),
        ]
        problem = Problem(blocksworld_domain, objects, positive_goals, negative_goals)
        data = [
            ProblemStates(problem, []),
        ]
        dataset = Dataset(blocksworld_domain, data)
        print("no exception caught")
    except RuntimeError as e:
        correct_exception_caught = str(e).startswith("Arity mismatch")
        print(f"exception caught: {e}")
    assert correct_exception_caught


def test_bad_predicate_in_state():
    try:
        state = [
            Atom(lime, ["a", "b", "c", "d", "e"]),
        ]
        problem = Problem(blocksworld_domain, objects, positive_goals, negative_goals)
        data = [
            ProblemStates(problem, [state]),
        ]
        dataset = Dataset(blocksworld_domain, data)
        print("no exception caught")
    except RuntimeError as e:
        correct_exception_caught = str(e).startswith("Unknown predicate")
        print(f"exception caught: {e}")
    assert correct_exception_caught


def test_bad_object_in_state():
    try:
        state = [
            Atom(on, ["asdfasdfa", "basdfasdf"]),
        ]
        problem = Problem(blocksworld_domain, objects, positive_goals, negative_goals)
        data = [
            ProblemStates(problem, [state]),
        ]
        dataset = Dataset(blocksworld_domain, data)
        print("no exception caught")
    except RuntimeError as e:
        correct_exception_caught = str(e).startswith("Unknown object")
        print(f"exception caught: {e}")
    assert correct_exception_caught


def test_bad_arity_in_state():
    try:
        state = [
            Atom(on, ["a", "b", "c"]),
        ]
        problem = Problem(blocksworld_domain, objects, positive_goals, negative_goals)
        data = [
            ProblemStates(problem, [state]),
        ]
        dataset = Dataset(blocksworld_domain, data)
        print("no exception caught")
    except RuntimeError as e:
        correct_exception_caught = str(e).startswith("Arity mismatch")
        print(f"exception caught: {e}")
    assert correct_exception_caught
