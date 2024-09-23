import time
from itertools import product

import numpy as np

from wlplan.data import Dataset, ProblemStates
from wlplan.feature_generation import WLFeatures
from wlplan.planning import Atom, Domain, Predicate, Problem

## domain
on = Predicate("on", 2)
on_table = Predicate("on-table", 1)
clear = Predicate("clear", 1)
holding = Predicate("holding", 1)
arm_empty = Predicate("arm-empty", 0)
predicates = [on, on_table, clear, holding, arm_empty]
constant_objects = ["dummy_constant_block"]

domain = Domain(
    name="blocksworld",
    predicates=predicates,
    constant_objects=constant_objects,
)
print(f"{domain=}")

## problem 0
print("problem 0")
objects = ["a", "b", "c", "d", "e", "f", "g"]
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

problem1 = Problem(domain, objects, positive_goals, negative_goals)

# a
# b d f
# c e g
state11 = [
    Atom(clear, ["a"]),
    Atom(on, ["a", "b"]),
    Atom(on, ["b", "c"]),
    Atom(on_table, ["c"]),
    Atom(clear, ["d"]),
    Atom(on, ["d", "e"]),
    Atom(on_table, ["e"]),
    Atom(clear, ["f"]),
    Atom(on, ["f", "g"]),
    Atom(on_table, ["g"]),
]

## problem 1
print("problem 1")
objects = ["a", "b", "c"]
# a
# b c
positive_goals = [
    Atom(clear, ["a"]),
    Atom(on, ["a", "b"]),
    Atom(on_table, ["b"]),
    Atom(clear, ["c"]),
    Atom(on_table, ["c"]),
]
negative_goals = []

print(f"{positive_goals=}")
print(f"{negative_goals=}")

problem2 = Problem(domain, objects, positive_goals, negative_goals)

# a b c
state21 = [
    Atom(clear, ["a"]),
    Atom(on_table, ["a"]),
    Atom(clear, ["b"]),
    Atom(on_table, ["b"]),
    Atom(clear, ["c"]),
    Atom(on_table, ["c"]),
]

# a
# b
# c
state22 = [
    Atom(clear, ["a"]),
    Atom(on, ["a", "b"]),
    Atom(on, ["b", "c"]),
    Atom(on_table, ["c"]),
]

## dataset
data = [
    ProblemStates(problem1, [state11]),
    ProblemStates(problem2, [state21, state22]),
    # ProblemStates(problem2, [state21]),
]
dataset = Dataset(domain=domain, data=data)

data_repeated = [
    ProblemStates(problem1, [state11]),
    ProblemStates(problem1, [state11]),
    ProblemStates(problem1, [state11]),
    ProblemStates(problem2, [state21, state22]),
    ProblemStates(problem2, [state21, state21, state22]),
    ProblemStates(problem2, [state21, state22, state22]),
    ProblemStates(problem2, [state21, state21, state21]),
    ProblemStates(problem2, [state21]),
    ProblemStates(problem2, [state22]),
    ProblemStates(problem2, [state22]),
]
dataset_repeated = Dataset(domain=domain, data=data_repeated)

## features
ITERATIONS = 4
PROFILE = False
PROFILE_REPEATS = 100000


def test_prune_features():
    feature_generators = {}
    Xs = {}
    for prune_features in [None, "collapse", "collapse_by_layer"]:
        print("=" * 80)
        print(f"prune_features={prune_features}")
        feature_generator = WLFeatures(
            domain,
            graph_representation="ilg",
            iterations=ITERATIONS,
            prune_features=prune_features,
            multiset_hash=False,
        )
        feature_generator.collect(dataset)
        X = np.array(feature_generator.embed(dataset))
        X_unique = np.unique(X, axis=1)
        feature_generators[prune_features] = feature_generator
        Xs[prune_features] = X
        print(f"{X.shape=}")
        # print(X)
        print(f"{X_unique.shape=}")
        # print(X_unique)

        if PROFILE:
            t = time.time()
            for _ in range(PROFILE_REPEATS):
                X = np.array(feature_generator.embed(dataset))
            t = time.time() - t
            print(f"{t=:.3f}s")

    assert Xs["collapse"].shape <= Xs["collapse_by_layer"].shape
    assert Xs["collapse"].shape <= Xs[None].shape
    assert Xs["collapse_by_layer"].shape <= Xs[None].shape


def test_repeated_dataset():
    for prune_features in [None, "collapse", "collapse_by_layer"]:
        print("=" * 80)
        print(prune_features)
        column_sizes = set()
        feature_generator = WLFeatures(
            domain,
            graph_representation="ilg",
            iterations=ITERATIONS,
            prune_features=prune_features,
            multiset_hash=False,
        )
        feature_generator.collect(dataset)

        feature_generator_repeated = WLFeatures(
            domain,
            graph_representation="ilg",
            iterations=ITERATIONS,
            prune_features=prune_features,
            multiset_hash=False,
        )
        feature_generator_repeated.collect(dataset_repeated)

        for d, fg in product(
            [dataset, dataset_repeated],
            [feature_generator, feature_generator_repeated],
        ):
            X = np.array(fg.embed(d))
            column_sizes.add(X.shape[1])
            print(f"{X.shape=}")

        assert len(column_sizes) == 1

def test_multiset():
    feature_generators = {}
    Xs = {}
    for prune_features in [None, "collapse", "collapse_by_layer"]:
        print("=" * 80)
        print(f"prune_features={prune_features}")
        feature_generator = WLFeatures(
            domain,
            graph_representation="ilg",
            iterations=ITERATIONS,
            prune_features=prune_features,
            multiset_hash=True,
        )
        feature_generator.collect(dataset)
        X = np.array(feature_generator.embed(dataset))
        X_unique = np.unique(X, axis=1)
        feature_generators[prune_features] = feature_generator
        Xs[prune_features] = X
        print(f"{X.shape=}")
        # print(X)
        print(f"{X_unique.shape=}")
        # print(X_unique)

        if PROFILE:
            t = time.time()
            for _ in range(PROFILE_REPEATS):
                X = np.array(feature_generator.embed(dataset))
            t = time.time() - t
            print(f"{t=:.3f}s")

    assert Xs["collapse"].shape <= Xs["collapse_by_layer"].shape
    assert Xs["collapse"].shape <= Xs[None].shape
    assert Xs["collapse_by_layer"].shape <= Xs[None].shape
