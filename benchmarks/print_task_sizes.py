import os

from domains import DOMAINS

print(r"domain & train size & test size \\")
for domain in DOMAINS:
    test_dir = f"{domain}/numeric/testing"
    plans_dir = f"{domain}/numeric/training_plans"
    tr_task_sizes = []
    te_task_sizes = []

    def get_objects(output):
        objects = output[output.find("(:objects") + 9 :]
        objects = objects[: objects.find(")")]
        return objects
    
    def get_init(output):
        init = output[output.find("(:init") + 6 :]
        init = init[: init.find("(:goal")]
        while "  " in init:
            init = init.replace("  ", " ")
        init = init.replace("\n", "")
        while init.endswith(" "):
            init = init[:-1]
        init = init.strip()
        init = init.split(") (")
        for i, val in enumerate(init):
            if val.startswith("("):
                init[i] = val[1:]
            elif val.endswith(")"):
                init[i] = val[:-1]
            else:
                init[i] = val
        return init
    
    def get_goal(output):
        init = output[output.find("(:goal (and (") + len("(:goal (and (") :]
        while "  " in init:
            init = init.replace("  ", " ")
        init = init.replace("\n", "")
        while init.endswith(" "):
            init = init[:-1]
        init = init.strip()
        init = init.split(") (")
        for i, val in enumerate(init):
            # print(val)
            if val.startswith("("):
                init[i] = val[1:]
            elif val.endswith(")"):
                init[i] = val[:-1]
            else:
                init[i] = val
        return init
    
    def _helper_from_objects(output, obj_type):
        objects = get_objects(output)
        for line in objects.split("\n"):
            if obj_type in line:
                return len(line.split()) - 2
        raise RuntimeError

    def blocksworld_task_size(output):
        return _helper_from_objects(output, "block")

    def childsnack_task_size(output):
        init = get_init(output)
        ret = 0
        for tok in init:
            if tok.startswith("= (hungry"):
                ret += int(tok.split()[-1])
        return ret
    
    def ferry_task_size(output):
        return _helper_from_objects(output, "car")
    
    def miconic_task_size(output):
        return _helper_from_objects(output, "passenger")
    
    def rovers_task_size(output):
        goal = get_goal(output)
        ret = 0
        for tok in goal:
            if tok.startswith("communicated"):
                ret += 1
        return ret
    
    def satellite_task_size(output):
        return _helper_from_objects(output, "dir")
    
    def spanner_task_size(output):
        init = get_init(output)
        ret = 0
        for tok in init:
            if tok.startswith("= (spanners_at"):
                ret += int(tok.split()[-1])
        return ret
    
    def transport_task_size(output):
        return _helper_from_objects(output, "package")
    
    mapper = {
        "blocksworld": blocksworld_task_size,
        "childsnack": childsnack_task_size,
        "ferry": ferry_task_size,
        "miconic": miconic_task_size,
        "rovers": rovers_task_size,
        "satellite": satellite_task_size,
        "spanner": spanner_task_size,
        "transport": transport_task_size,
    }

    for file in os.listdir(plans_dir):
        if not file.endswith(".plan"):
            continue
        pddl = f"{domain}/numeric/training/{file.split('.')[0]}.pddl"
        with open(pddl, "r") as f:
            output = f.read()
        tr_task_sizes.append(mapper[domain](output))
    
    for file in os.listdir(test_dir):
        if not file.endswith(".pddl"):
            continue
        p_name = file.split(".")[0]
        pddl = f"{test_dir}/{file}"
        with open(pddl, "r") as f:
            output = f.read()
        te_task_sizes.append(mapper[domain](output))

    if len(tr_task_sizes) == 0:
        tr_task_sizes = [0]
    if len(te_task_sizes) == 0:
        te_task_sizes = [0]

    tr_t_size = f"[{min(tr_task_sizes)}, {max(tr_task_sizes)}]"
    te_t_size = f"[{min(te_task_sizes)}, {max(te_task_sizes)}]"
    ret = f"{domain} & {tr_t_size} & {te_t_size} \\\\"
    print(ret)
