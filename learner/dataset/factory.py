import os
from dataclasses import dataclass
from typing import Dict, Iterable, List, Union
from tqdm import tqdm
from dlplan.state_space import generate_state_space, GeneratorExitCode
from dlplan.core import State as DLPlanState, VocabularyInfo

_DOWNWARD = "./../planners/downward_cpu/fast-downward.py"
_POWERLIFTED = "./../planners/powerlifted/powerlifted.py"

ALL_KEY = "_all_"

MAX_NUM_STATES = 10000

State = Union[DLPlanState, Iterable[str]]


@dataclass
class StateCostData:
    state: State
    cost: Dict[str, float]  # schema count -> cost
    domain_pddl: str
    problem_pddl: str


@dataclass
class StateCostDataset:
    state_cost_data: List[StateCostData]
    vocabulary_info: VocabularyInfo


def group_by_problem(dataset: StateCostDataset) -> Dict[str, List[StateCostData]]:
    ret = {}
    for data in dataset.state_cost_data:
        if data.problem_pddl not in ret:
            ret[data.problem_pddl] = []
        ret[data.problem_pddl].append(data)
    return ret


def state_cost_dataset_from_plans(
    domain_pddl, tasks_dir, plans_dir
) -> StateCostDataset:
    state_cost_data = []

    for plan_file in tqdm(sorted(list(os.listdir(plans_dir)))):
        problem_pddl = f"{tasks_dir}/{plan_file.replace('.plan', '.pddl')}"
        assert os.path.exists(problem_pddl), problem_pddl
        plan_file = f"{plans_dir}/{plan_file}"
        plan = _get_states_from_plan(domain_pddl, problem_pddl, plan_file)
        state_cost_data += plan

    dataset = StateCostDataset(
        state_cost_data=state_cost_data, vocabulary_info=None
    )
    return dataset


def _get_states_from_plan(
    domain_pddl, problem_pddl, plan_file
) -> List[StateCostData]:
    states = []
    actions = []

    planner = "fd"  # TODO fix this with statics
    # planner = args.planner

    with open(plan_file, "r") as f:
        for line in f.readlines():
            if ";" in line:
                continue
            actions.append(line.replace("\n", ""))

    state_file = (
        repr(hash(domain_pddl))
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
    if os.popen(cmd).readlines() is None:
        pass  # make this seen by linter
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
    # ignore the goal state, annoying for learning useful schema
    for i, state in enumerate(states[:-1]):
        action = actions[i]
        schema = action.replace("(", "").split()[0]
        data = StateCostData(
            state=state,
            cost=schema_cnt.copy(),
            domain_pddl=domain_pddl,
            problem_pddl=problem_pddl,
        )
        ret.append(data)
        schema_cnt[schema] -= 1
        schema_cnt[ALL_KEY] -= 1
    return ret


def state_cost_dataset_from_spaces(domain_pddl, tasks_dir) -> StateCostDataset:
    state_cost_data = []

    state_space = generate_state_space(
        domain_pddl,
        f"{tasks_dir}/{sorted(list(os.listdir(tasks_dir)))[0]}",
        index=0,
        max_num_states=1,
    ).state_space
    instance_info = state_space.get_instance_info()
    vocabulary_info = instance_info.get_vocabulary_info()

    collected_from = []
    states = 0

    for f in tqdm(sorted(list(os.listdir(tasks_dir)))):
        problem_pddl = f"{tasks_dir}/{f}"
        generator = generate_state_space(
            domain_file=domain_pddl,
            instance_file=problem_pddl,
            vocabulary_info=vocabulary_info,
            index=0,
            max_num_states=MAX_NUM_STATES,
        )

        if generator.exit_code != GeneratorExitCode.COMPLETE:
            continue

        collected_from.append(problem_pddl)
        state_space = generator.state_space

        # collect distance for all states
        instance_info = state_space.get_instance_info()

        goal_dist = state_space.compute_goal_distances()
        for s_id, state in state_space.get_states().items():
            # non dead end states only
            if s_id in goal_dist:
                data = StateCostData(
                    state,
                    {ALL_KEY: goal_dist[s_id]},
                    domain_pddl,
                    problem_pddl,
                )
                state_cost_data.append(data)

    n_states = len(state_cost_data)
    print(f"Collected {n_states} states from {len(collected_from)} problems:")
    print("\n".join(collected_from))

    dataset = StateCostDataset(state_cost_data, vocabulary_info)

    return dataset
