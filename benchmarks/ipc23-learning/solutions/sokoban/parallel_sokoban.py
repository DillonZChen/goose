from benchmarking_utils import parallel_execution
from sokoban import parse_plan, apply_plan
from unified_planning.io import PDDLReader


def solve_problem(domain, problem, plan_file):
    print(f"Solving {problem}...")
    reader = PDDLReader()
    pddl_problem = reader.parse_problem(domain, problem)
    plan = parse_plan(pddl_problem, plan_file)
    if not apply_plan(pddl_problem, plan):
        print(f"Problem {problem} failed!")
        return False
    return True


def main():
    all_problems = [
        f"../../sokoban/training/easy/p{p:02}.pddl" for p in range(1, 100)
    ]
    all_problems.extend(
        [f"../../sokoban/testing/easy/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [f"../../sokoban/testing/medium/p{p:02}.pddl" for p in range(1, 31)]
    )
    all_problems.extend(
        [f"../../sokoban/testing/hard/p{p:02}.pddl" for p in range(1, 31)]
    )

    all_plans = [f"training/easy/p{p:02}.plan" for p in range(1, 100)]
    all_plans.extend([f"testing/easy/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/medium/p{p:02}.plan" for p in range(1, 31)])
    all_plans.extend([f"testing/hard/p{p:02}.plan" for p in range(1, 31)])

    parallel_execution(
        solve_problem, "../../sokoban/domain.pddl", all_problems, all_plans
    )


if __name__ == "__main__":
    main()
