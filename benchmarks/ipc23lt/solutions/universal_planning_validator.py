from benchmarking_utils import execute_command
import json


def main():
    # Run the universal-planning-validator (UPV) on all domains / instances and plans
    upv_dir = "../../../planning-validators/universal-planning-validator/validator/validate.bin"
    domains = [
        "blocksworld",
        "childsnack",
        "ferry",
        "floortile",
        "miconic",
        "rovers",
        "satellite",
        "sokoban",
        "spanner",
        "transport",
    ]
    subfolders = ["testing/easy", "testing/medium", "testing/hard"]
    problems = [f"p{p:02}" for p in range(1, 31)]

    failures = dict()
    plan_costs = dict()
    for domain in domains:
        d_file = f"../{domain}/domain.pddl"
        for folder in subfolders:
            for problem in problems:
                prob_file = f"../{domain}/{folder}/{problem}.pddl"
                plan_file = f"{domain}/{folder}/{problem}.plan"
                command = f"{upv_dir} {d_file} {prob_file} {plan_file}"

                ret_code = execute_command(command=command.split())

                if ret_code != 0:  # failure!
                    failures[command] = ret_code

                # Ignore if it is a failure
                cost = 0
                with open(plan_file) as pf:
                    for line in pf.readlines():
                        if line and line[0] != ";":
                            cost += 1
                plan_costs[f"{prob_file[3:]}"] = cost

    if failures:
        for k, v in failures.items():
            print(f"Ret code={v}; command={k}")
    else:
        print("All plans are correct!")

    # Create JSON file with plan_cost
    with open("plan_costs.json", "w") as jf:
        json.dump(plan_costs, jf)


if __name__ == "__main__":
    main()
