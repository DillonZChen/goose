import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))

import matplotlib.pyplot as plt
import os
import time
import torch
import argparse
import torch_geometric
import numpy as np

from matplotlib.pyplot import cm
from models import *
from tqdm import tqdm, trange

from representation import add_features
from util.save_load import load_model
from gen_data import get_graph_data
from sklearn.manifold import TSNE
from sklearn.decomposition import PCA

from util.transform import preprocess_data


def main():
  print("TODO: have a look at plotly")

  parser = argparse.ArgumentParser()

  parser.add_argument('-m', type=str, help='path to model params file', required=True)
  parser.add_argument('-n', '--max_nodes', type=int, help="max node for generating graphs (-1 for no bound)", required=True)
  parser.add_argument('-c', '--cutoff', type=int, help="max cost to learn (-1 for no bound)", required=True)
  parser.add_argument('-d', '--dim_red', type=str, choices=["pca", "tsne"], default="pca")
  parser.add_argument('--load', action='store_true')
  parser.add_argument('--device', type=int, default=0)

  main_args = parser.parse_args()

  model_path = main_args.m
  max_nodes = main_args.max_nodes
  cutoff = main_args.cutoff
  dim_red_method = main_args.dim_red

  # cuda
  device = torch.device(f'cuda:{main_args.device}' if torch.cuda.is_available() else 'cpu')

  data_file = f"logs/C{cutoff}_N{max_nodes}_M{model_path}.data"
  load = main_args.load and os.path.exists(data_file)
  if load:
    embeddings, emb_by_domain, max_y, emb_by_y = torch.load(f=data_file)
  else:
    model, args = load_model(model_path, print_args=False)
    model = model.to(device)

    dataset = get_graph_data(representation=args.rep, small_train=False, task=args.task)
    dataset = preprocess_data(model_name=args.model, data_list=dataset, cutoff=cutoff, max_nodes=max_nodes, task=args.task)
    dataset = add_features(dataset, args)

    emb_by_domain = {}
    emb_by_y = {}
    max_y = 0
    embeddings = []
    for i in trange(len(dataset)):
      data = dataset[i]
      domain = data.domain
      y = data.y
      x = data.x.to(device)
      edge_index = data.edge_index.to(device)
      if domain not in emb_by_domain:
        emb_by_domain[domain] = []
      if y not in emb_by_y:
        emb_by_y[y] = []
      v = model.graph_embedding(x, edge_index).detach().cpu()
      embeddings.append(v)
      emb_by_domain[domain].append(i)
      emb_by_y[y].append(i)
      max_y = max(max_y, y)

    data_to_save = (embeddings, emb_by_domain, max_y, emb_by_y)
    torch.save(data_to_save, f=data_file)

  emb = np.array([np.array(v[0]) for v in embeddings])

  if dim_red_method=="pca":
    pca = PCA(n_components=2)
    pca.fit(emb)
    X = pca.transform(emb)
    print(X)
  else:
    X = TSNE(n_components=2, learning_rate='auto', init='random', perplexity=3).fit_transform(emb)

  color = cm.rainbow(np.linspace(0, 1, len(emb_by_domain)))
  for d, c in zip(emb_by_domain, color):
    plt.scatter([X[i][0] for i in emb_by_domain[d]],
                [X[i][1] for i in emb_by_domain[d]],
                label=d, s=9, c=c)
  plt.title("emb by domain")
  plt.legend(fontsize = 'xx-small')
  plt.xscale("symlog")
  plt.yscale("symlog")
  plt.savefig(f"plots/domain_{dim_red_method}", dpi=960)
  plt.clf()

  color = cm.rainbow(np.linspace(0, 1, len(emb_by_y)))
  for y, c in zip(range(int(max_y)+1), color):
    try:
      plt.scatter([X[i][0] for i in emb_by_y[y]],
                  [X[i][1] for i in emb_by_y[y]],
                  label=y, s=9, c=c)
    except Exception as e:
      pass
  plt.title("emb by y")
  # plt.legend(fontsize = 'xx-small')
  plt.xscale("symlog")
  plt.yscale("symlog")
  plt.savefig(f"plots/y_{dim_red_method}", dpi=960)
  plt.clf()

if __name__ == "__main__":
  main()
