# Import the PDDLReader and PDDLWriter classes
from unified_planning.io import PDDLReader
from unified_planning.shortcuts import SequentialSimulator
from unified_planning.plans import SequentialPlan, ActionInstance
from unified_planning.model.walkers import StateEvaluator


def apply_plan(pddl_problem, plan) -> bool:
    goal = pddl_problem.goals[0]

    with SequentialSimulator(problem=pddl_problem) as simulator:
        state = simulator.get_initial_state()
        se = StateEvaluator(simulator._problem)
        for act in plan.actions:
            if not simulator.is_applicable(state, act):
                print("Invalid plan :-(")
                return False
            state = simulator.apply(state, act)
        if se.evaluate(goal, state).bool_constant_value():
            print("Goal achieved!")
            return True

        print("Invalid plan :-(")
        return False


def parse_plan(pddl_problem, plan_file) -> SequentialPlan:
    plan = list()
    # print(pddl_problem.user_types)  # [location, direction, box]
    utypes = pddl_problem.user_types
    actions = (
        pddl_problem.actions
    )  # move(from, to, dir); push(rloc, bloc, floc, dir, box)
    # print(actions)

    with open(plan_file) as f:
        for line in f.readlines():
            if line[0] == ";":
                continue
            act_tokens = line[
                1:-2
            ].split()  # skip '(' and ')', and split by whitespaces
            args = [pddl_problem.object(o) for o in act_tokens[1:]]
            if act_tokens[0] == "move":
                plan.append(ActionInstance(actions[0], args))
            elif act_tokens[0] == "push":
                plan.append(ActionInstance(actions[1], args))
            else:
                raise f"Error: action {act_tokens[0]} unrecognized!"

    return SequentialPlan(plan)


def main():
    # plan_test()
    reader = PDDLReader()
    """
    pddl_problem = reader.parse_problem('../../sokoban/domain.pddl', '../../sokoban/training/easy/p04.pddl')
    plan = parse_plan(pddl_problem, 'training/easy/p04.plan')
    print(plan)
    print(f"Is valid? {apply_plan(pddl_problem, plan)}")
    """
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

    for prob, plan_file in zip(all_problems, all_plans):
        print(f"Solving {prob}...")
        pddl_problem = reader.parse_problem("../../sokoban/domain.pddl", prob)
        plan = parse_plan(pddl_problem, plan_file)
        if not apply_plan(pddl_problem, plan):
            print(f"Problem {prob} failed!")


# """


if __name__ == "__main__":
    main()
