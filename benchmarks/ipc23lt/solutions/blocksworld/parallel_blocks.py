import argparse
import logging
import os
import sys
from benchmarking_utils import parallel_execution
from blocksworld import generalize_plan, apply_plan
from unified_planning.io import PDDLReader


def solve_problem(domain, problem, plan_file, check_validity: bool = False):
    print(f"Solving {problem}...")
    reader = PDDLReader()
    pddl_problem = reader.parse_problem(domain, problem)
    plan = generalize_plan(pddl_problem)
    if check_validity and (not apply_plan(pddl_problem, plan)):
        print(f"Problem {problem} failed!")
        return False
    with open(plan_file, "w") as f:
        for act in plan._actions:
            f.write(
                f"({act._action._name} {' '.join([str(arg) for arg in act._params])})\n"
            )
        f.write(f"; cost = {len(plan._actions)} (unit cost)")
    return True


def main():
    os.makedirs("training/easy/", exist_ok=True)
    os.makedirs("testing/easy/", exist_ok=True)
    os.makedirs("testing/medium/", exist_ok=True)
    os.makedirs("testing/hard/", exist_ok=True)

    all_problems = [
        f"../../blocksworld/training/easy/p{p:02}.pddl" for p in range(1, 100)
    ]
    all_problems.extend(
        [f"../../blocksworld/testing/easy/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [
            f"../../blocksworld/testing/medium/p{p:02}.pddl"
            for p in range(1, 31)
        ]
    )
    all_problems.extend(
        [f"../../blocksworld/testing/hard/p{p:02}.pddl" for p in range(1, 31)]
    )

    all_plans = [f"training/easy/p{p:02}.plan" for p in range(1, 100)]
    all_plans.extend([f"testing/easy/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/medium/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/hard/p{p:02}.plan" for p in range(1, 31)])

    parallel_execution(
        solve_problem, "../../blocksworld/domain.pddl", all_problems, all_plans
    )


if __name__ == "__main__":
    main()
