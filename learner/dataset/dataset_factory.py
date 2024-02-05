import os
import random

_DOWNWARD = "./../planners/downward_cpu/fast-downward.py"
_POWERLIFTED = "./../planners/powerlifted/powerlifted.py"

ALL_KEY = "_all_"


def sample_from_dict(d, sample, seed):
    random.seed(seed)
    keys = random.sample(list(d), sample)
    values = [d[k] for k in keys]
    return dict(zip(keys, values))


def collect_states_from_plan(domain_pddl, problem_pddl, plan_file, args):
    states = []
    actions = []

    planner = args.planner

    with open(plan_file, "r") as f:
        for line in f.readlines():
            if ";" in line:
                continue
            actions.append(line.replace("\n", ""))

    state_file = (
        repr(hash(repr(args)))
        + repr(hash(domain_pddl))
        + repr(hash(problem_pddl))
        + repr(hash(plan_file))
    )
    state_file = state_file.replace("-", "0")
    aux_file = state_file + ".sas"
    state_file = state_file + ".states"

    cmd = {
        "pwl": f"export PLAN_PATH={plan_file} "
        + f"&& {_POWERLIFTED} -d {domain_pddl} -i {problem_pddl} -s perfect "
        + f"--plan-file {state_file}",
        "fd": f"export PLAN_INPUT_PATH={plan_file} "
        + f"&& export STATES_OUTPUT_PATH={state_file} "
        + f"&& {_DOWNWARD} --sas-file {aux_file} {domain_pddl} {problem_pddl} "
        + f"--search 'perfect([blind()])'",  # need filler h
    }[planner]

    # print("generating plan states with:") print(cmd)

    # disgusting method which hopefully makes running in parallel work fine
    assert not os.path.exists(aux_file), aux_file
    assert not os.path.exists(state_file), state_file
    output = os.popen(cmd).readlines()
    if output == None:
        pass  # make this variable seen by linter
    if os.path.exists(aux_file):
        os.remove(aux_file)

    if not os.path.exists(state_file):
        err_msg = f"Failed to generate states from training data plans. This may be because you did not build the planners yet. This can be done with\n\n\tsh build_components.sh\n"
        raise RuntimeError(err_msg)

    with open(state_file, "r") as f:
        for line in f.readlines():
            if ";" in line:
                continue
            line = line.replace("\n", "")
            s = set()
            for fact in line.split():
                if "(" not in fact:
                    lime = f"({fact})"
                else:
                    pred = fact[: fact.index("(")]
                    fact = fact.replace(pred + "(", "").replace(")", "")
                    args = fact.split(",")[:-1]
                    lime = "(" + " ".join([pred] + args) + ")"
                s.add(lime)
            states.append(sorted(list(s)))
    os.remove(state_file)

    schema_cnt = {ALL_KEY: len(actions)}
    for action in actions:
        schema = action.replace("(", "").split()[0]
        if schema not in schema_cnt:
            schema_cnt[schema] = 0
        schema_cnt[schema] += 1

    ret = []
    for i, state in enumerate(states):
        if i == len(actions):
            continue  # ignore the goal state, annoying for learning useful schema
        action = actions[i]
        schema = action.replace("(", "").split()[0]
        ret.append((state, schema_cnt.copy()))
        # print(state)
        # for s, v in schema_cnt.items():
        #     print(f"{s:>15} {v:>4}")
        # print()
        schema_cnt[schema] -= 1
        schema_cnt[ALL_KEY] -= 1
    # breakpoint()
    return ret
