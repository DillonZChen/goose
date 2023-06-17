import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
import time
import util.ipc_domain_info as ipc_domain_info
import util.htg_domain_info as htg_domain_info
import util.goose_domain_info as goose_domain_info
from representation import REPRESENTATIONS
from util.htg_domain_info import get_all_htg_instance_files
from util.ipc_domain_info import same_domain, GROUNDED_DOMAINS, get_ipc_domain_problem_files
from util.goose_domain_info import get_train_hgn_instance_files
from gen_data.stats import *

def generate_graph_from_domain_problem_pddl(domain_name: str,
                                            domain_pddl: str,
                                            problem_pddl: str,
                                            representation: str,
                                            task: str):
  """ generates a list of graphs corresponding to states in the optimal plan """
  ret = []
  del_free = False

  plan = optimal_plan_exists(domain_name, domain_pddl, problem_pddl, del_free)
  if plan is None:
    return None
  
  rep = REPRESENTATIONS[representation](domain_pddl=domain_pddl,
                                        problem_pddl=problem_pddl)
  # try:
  #   rep = REPRESENTATIONS[representation](domain_pddl=domain_pddl,
  #                                         problem_pddl=problem_pddl)
  # except AssertionError:
  #   print(f"Skipping generation of {representation} for {domain_name}")
  #   return None

  problem_name = os.path.basename(problem_pddl).replace(".pddl", "")

  for s, y, a in plan:
    if representation in {"ldg", "ldg-el"}:
      s = rep.str_to_state(s)
    output = rep.get_state_enc(s)
    # assert 0
    if output is None:
      continue
    # if CONFIG[representation]["edge_labels"]==True:
    #   x, edge_index, edge_type = output
    # else:
    #   x, edge_index = output
    #   edge_type = None
    x, edge_index = output
    if task=="h":
      applicable_action=None
    elif task=="a":
      tqdm.write("not implemented task prediction for all graph reps yet")
      raise NotImplementedError
      # y = torch.zeros(rep.n_applicable_action)
      # y[rep.name_mapping[a]] = 1
    else:
      raise ValueError(f"Unexpected task {task}")

    tag = hash((domain_name, problem_name, y))

    heuristics = {}  # just copy from precomputed elpdg
    # t = time.time()
    # for h in HEURISTICS:
    #   heuristics[h] = HEURISTICS[h](problem)(searchspace.make_root_node(s))
    # t = time.time() - t
    # tqdm.write(f"heuristics computed in {t:.4f}s")

    graph_data = Data(x=x,
                      edge_index=edge_index,
                      a=a,
                      # edge_type=edge_type,
                      y=y,
                      domain=domain_name,
                      problem=problem_name,
                      applicable_action=applicable_action,
                      state=s,
                      heuristics=heuristics,
                      hash=tag,
                      )
    ret.append(graph_data)
  return ret


def get_graph_data(
     representation: str,
     task: str,
     domain: str="all",
) -> List[Data]:
  """ Get lots of data generated from generate_data """
  print("Loading train data...")
  print("NOTE: the data has been precomputed and saved.")
  print("Rerun gen_data/graphs.py if representation has been updated!")
  ret = []
  path = get_data_dir_path(representation=representation, task=task)
  print(f"Path to data: {path}")
  for domain_name in tqdm(sorted(list(os.listdir(path)))):
    if ".data" in domain_name:
      continue
    if domain_name in ipc_domain_info.GENERAL_COST_DOMAINS or domain_name in htg_domain_info.GENERAL_COST_DOMAINS:
      # tqdm.write(f"\t{domain_name} skipped since it does not have unit costs")
      continue
    if domain == "all":
      pass  # accept everything
    elif domain == "ipc-only":  # codebase getting bloated
      if "ipc-" not in domain_name:
        continue
    elif domain == "goose-pretraining":  # ipc + goose
      if domain_name in goose_domain_info.DOMAINS_NOT_TO_TRAIN or "htg-" in domain_name:
        continue
    elif domain == "goose-unseen-pretraining":  # ipc only
      if domain_name in goose_domain_info.DOMAINS_NOT_TO_TRAIN or "htg-" in domain_name or "goose-" in domain_name:
        continue
    else:
      if "-only" not in domain and not same_domain(domain, domain_name):
          continue
      elif "-only" in domain and domain.replace("-only", "")!=domain_name:
          continue
      
    for data in sorted(list(os.listdir(f"{path}/{domain_name}"))):
      next_data = torch.load(f'{path}/{domain_name}/{data}')
      ret+=next_data

  print(f"{domain} dataset of size {len(ret)} loaded!")
  return ret

def generate_graph_rep_domain(domain_name: str,
                              domain_pddl: str,
                              problem_pddl: str,
                              representation: str,
                              task: str,
                              regenerate: bool) -> bool:
  """ Saves list of torch_geometric.data.Data of graphs and features to file. """
  save_file = get_data_path(domain_name,
                            domain_pddl,
                            problem_pddl,
                            representation,
                            task)
  if os.path.exists(save_file):
    if not regenerate:
      return 0
      # return torch.load(save_file)
    else:
      os.remove(save_file)  # make a fresh set of data
  
  graph = generate_graph_from_domain_problem_pddl(domain_name=domain_name,
                                                  domain_pddl=domain_pddl,
                                                  problem_pddl=problem_pddl,
                                                  representation=representation,
                                                  task=task)
  if graph is not None:
    tqdm.write(f'saving data @{save_file}...')
    torch.save(graph, save_file)
    tqdm.write('data saved!')
    return 1
  return 0


def gen_graph_rep(representation: str,
                  regenerate: bool):
  """ Generate graph representations from saved optimal plans. """

  # TASKS = ["h", "a"]
  TASKS = ["h"]

  tasks  = get_ipc_domain_problem_files(del_free=False)
  # tasks += get_all_htg_instance_files(split=True)
  tasks += get_train_hgn_instance_files()
  # tasks = get_train_hgn_instance_files()
  # tasks = get_all_htg_instance_files(split=True)

  new_generated = 0
  pbar = tqdm(tasks)
  for domain_name, domain_pddl, problem_pddl in tasks:
    problem_name = os.path.basename(problem_pddl).replace(".pddl", "")
    # if representation in LIFTED_REPRESENTATIONS and domain_name in GROUNDED_DOMAINS:
    #   continue
    pbar.set_description(f"Generating {representation} graphs for {domain_name} {problem_name}")
    new_generated += generate_graph_rep_domain(domain_name=domain_name,
                                                domain_pddl=domain_pddl,
                                                problem_pddl=problem_pddl,
                                                representation=representation,
                                                regenerate=regenerate,
                                                task="h")
  print(f"newly generated graphs: {new_generated}")
  return


def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('-r', '--rep', type=str, help="graph representation to generate", choices=REPRESENTATIONS)
  parser.add_argument('--regenerate', action="store_true")
  args = parser.parse_args()

  rep = args.rep
  gen_graph_rep(representation=rep,
                regenerate=args.regenerate)
  return

if __name__ == "__main__":
  main()
