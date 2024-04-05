import os
import pickle
import time
from dataclasses import dataclass
from typing import Dict, Iterable, List, Tuple, Union

_DOWNWARD = "./../planners/downward_cpu/fast-downward.py"
_POWERLIFTED = "./../planners/powerlifted/powerlifted.py"
DATA_PATH = os.path.abspath("data/ipc23/")
ALL_KEY = "_all_"


MAX_NUM_STATES = 10000

State = Iterable[str]


@dataclass
class StateCostData:
    state: State
    cost: Dict[str, float]  # schema count -> cost
    domain_pddl: str
    problem_pddl: str


@dataclass
class StateRankData(StateCostData):
    loc: Tuple[int, int]


@dataclass
class StateCostDataset:
    state_cost_data_list: List[StateCostData]

    @property
    def schemata(self) -> List[str]:
        return list(self.state_cost_data_list[0].cost.keys())

    def save(self, path):
        for name, attribute in self.__dict__.items():
            name = ".".join((name, "pkl"))
            with open("/".join((path, name)), "wb") as f:
                pickle.dump(attribute, f)

    @classmethod
    def load(cls, path):
        my_model = {}
        for name in cls.__annotations__:
            file_name = ".".join((name, "pkl"))
            with open("/".join((path, file_name)), "rb") as f:
                my_model[name] = pickle.load(f)
        return cls(**my_model)


def reformat_y(y: List[Dict[str, float]]) -> Dict[str, List[float]]:
    schemata = list(y[0].keys())
    ret = {s: [] for s in schemata}
    for y_dict in y:
        for s in schemata:
            ret[s].append(y_dict[s] if s in y_dict else 0)
    return ret


def group_by_problem(
    dataset: StateCostDataset,
) -> Dict[str, List[StateCostData]]:
    ret = {}
    for data in dataset.state_cost_data_list:
        if data.problem_pddl not in ret:
            ret[data.problem_pddl] = []
        ret[data.problem_pddl].append(data)
    return ret


def state_cost_dataset_from_plans(
    domain_pddl,
    tasks_dir,
    plans_dir,
    planner="fd",
    load_from_memory=False,
) -> StateCostDataset:
    state_cost_data = []
    domain = domain_pddl.split('/')[-1].split('.')[0]
    data_path = os.path.join(DATA_PATH, f"{planner}-{domain}")
    if load_from_memory:
        print(f"loading data from {data_path}...")
        try:
            dataset = StateCostDataset.load(data_path)
            return dataset
        except FileNotFoundError:
            print("Dataset file not found. Have you generated the dataset before?")

    print("Generating data from plans...")
    t = time.time()
    for plan_file in sorted(list(os.listdir(plans_dir))):
        problem_pddl = f"{tasks_dir}/{plan_file.replace('.plan', '.pddl')}"
        assert os.path.exists(problem_pddl), problem_pddl
        plan_file = f"{plans_dir}/{plan_file}"

        # extract states with fd first
        states = []
        actions = []
        # planner = "fd"  # TODO fix this with statics
        # planner = args.planner

        with open(plan_file, "r") as f:
            for line in f.readlines():
                if ";" in line:
                    continue
                actions.append(line.replace("\n", ""))

        state_file = repr(hash(domain_pddl + problem_pddl + plan_file))
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
            "fd-rank": f"export PLAN_INPUT_PATH={plan_file} "
            + f"&& export STATES_OUTPUT_PATH={state_file} "
            + f"&& {_DOWNWARD} --sas-file {aux_file} {domain_pddl} {problem_pddl} "
            + f"--search 'perfect_with_siblings([blind()])'",
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

        if planner == "fd":
            # read states written into log file by fd
            with open(state_file, "r") as f:
                for line in f.readlines():
                    if ";" in line:
                        continue
                    line = line.replace("\n", "")

                    state = set()
                    for fact in line.split():
                        if "(" not in fact:
                            fact_str = f"({fact})"
                        else:
                            pred = fact[: fact.index("(")]
                            fact = fact.replace(pred + "(", "")
                            fact = fact.replace(")", "")
                            args = fact.split(",")[:-1]
                            fact_str = "(" + " ".join([pred] + args) + ")"
                        state.add(fact_str)
                    state = sorted(list(state))

                    states.append(state)

        elif planner == "fd-rank":
            # fd-rank returns a state file with {n-1} line breaks,
            # between line breaks are optimal states followed by their neighbors
            # so the first state after a line break (except the first state in the file)
            # is always the optimal
            with open(state_file, "r") as f:
                state_and_siblings = []
                for line in f.readlines():
                    if line == "\n":
                        states.append(state_and_siblings)
                        state_and_siblings = []
                        continue
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
                    state_and_siblings.append(s)

        os.remove(state_file)

        # collect distance, and also remaining schema count
        schema_cnt = {ALL_KEY: len(actions)}
        for action in actions:
            schema = action.replace("(", "").split()[0]
            if schema not in schema_cnt:
                schema_cnt[schema] = 0
            schema_cnt[schema] += 1

        # ignore the goal state, annoying for learning useful schema
        for i, state in enumerate(states[:-1]):
            action = actions[i]
            schema = action.replace("(", "").split()[0]
            if planner == "fd":
                data = StateCostData(
                    state=state,
                    cost=schema_cnt.copy(),
                    domain_pddl=domain_pddl,
                    problem_pddl=problem_pddl,
                )
                state_cost_data.append(data)
            elif planner == "fd-rank":
                # give each state a coordinate (i, j), where i is h* of the optimal state,
                # j=0 means the optimal, otherwise means neighbor of the optimal
                for j, neighbor_state in enumerate(state):
                    data = StateRankData(
                        state=neighbor_state,
                        cost=schema_cnt.copy(),
                        domain_pddl=domain_pddl,
                        problem_pddl=problem_pddl,
                        loc=(i, j),
                    )
                    state_cost_data.append(data)

            schema_cnt[schema] -= 1
            schema_cnt[ALL_KEY] -= 1
    print(f"Completed generating data from plans in {time.time()-t:.2f}s")
    print(f"{len(state_cost_data)} states collected.")

    dataset = StateCostDataset(state_cost_data_list=state_cost_data)
    os.makedirs(data_path, exist_ok=True)
    dataset.save(data_path)

    return dataset
