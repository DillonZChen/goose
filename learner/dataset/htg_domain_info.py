import os

# see https://ojs.aaai.org/index.php/AAAI/article/view/21206
HTG_DOMAINS = {
  "blocksworld": 40,
  "childsnack": 144,
  "genome-edit-distance": 312,
  "logistics": 40,
  "organic-synthesis": 56,
  "pipesworld-tankage-nosplit": 50,
  "rovers": 40,
  "visitall-multidimensional": 180,
}

HTG_DOMAINS_SPLIT = {
  "blocksworld-large-simple",
  "childsnack-contents-parsize1-cham3",
  "childsnack-contents-parsize1-cham5",
  "childsnack-contents-parsize1-cham7",
  "childsnack-contents-parsize2-cham3",
  "childsnack-contents-parsize2-cham5",
  "childsnack-contents-parsize2-cham7",
  "childsnack-contents-parsize3-cham3",
  "childsnack-contents-parsize3-cham5",
  "childsnack-contents-parsize3-cham7",
  "childsnack-contents-parsize4-cham3",
  "childsnack-contents-parsize4-cham5",
  "childsnack-contents-parsize4-cham7",
  "genome-edit-distance",
  "genome-edit-distance-positional",
  "logistics-large-simple",
  "organic-synthesis-MIT",
  "organic-synthesis-alkene",
  "organic-synthesis-original",
  "pipesworld-tankage-nosplit",
  "rovers-large-simple",
  "visitall-multidimensional-3-dim-visitall-CLOSE-g1",
  "visitall-multidimensional-3-dim-visitall-CLOSE-g2",
  "visitall-multidimensional-3-dim-visitall-CLOSE-g3",
  "visitall-multidimensional-3-dim-visitall-FAR-g1",
  "visitall-multidimensional-3-dim-visitall-FAR-g2",
  "visitall-multidimensional-3-dim-visitall-FAR-g3",
  "visitall-multidimensional-4-dim-visitall-CLOSE-g1",
  "visitall-multidimensional-4-dim-visitall-CLOSE-g2",
  "visitall-multidimensional-4-dim-visitall-CLOSE-g3",
  "visitall-multidimensional-4-dim-visitall-FAR-g1",
  "visitall-multidimensional-4-dim-visitall-FAR-g2",
  "visitall-multidimensional-4-dim-visitall-FAR-g3",
  "visitall-multidimensional-5-dim-visitall-CLOSE-g1",
  "visitall-multidimensional-5-dim-visitall-CLOSE-g2",
  "visitall-multidimensional-5-dim-visitall-CLOSE-g3",
  "visitall-multidimensional-5-dim-visitall-FAR-g1",
  "visitall-multidimensional-5-dim-visitall-FAR-g2",
  "visitall-multidimensional-5-dim-visitall-FAR-g3",
}

GENERAL_COST_DOMAINS = {
  "genome-edit-distance",
}

def get_domain_instance_pddl_for_domain(domain: str):
  ret = []
  for domain_dir in os.listdir("../dataset/htg"):
    if domain in domain_dir:
      dir_of_pddls = f"../dataset/htg/{domain_dir}"
      df = f"{dir_of_pddls}/domain.pddl"
      for file in os.listdir(dir_of_pddls):
        if file=="domain.pddl":
          continue
        pf = f"{dir_of_pddls}/{file}"
        ret.append((f'htg-{domain}', df, pf))
  return ret

def get_all_htg_instance_files(split=False):
  ret = []
  domains = HTG_DOMAINS_SPLIT if split else HTG_DOMAINS
  for domain in sorted(domains):
    if domain in GENERAL_COST_DOMAINS:
      continue
    ret += get_domain_instance_pddl_for_domain(domain)
  return ret
