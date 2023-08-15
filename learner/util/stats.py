import numpy as np
import os
import matplotlib.pyplot as plt
import torch
import networkx as nx
from sklearn.metrics import confusion_matrix, ConfusionMatrixDisplay
from tqdm import tqdm
from torch_geometric.utils.convert import to_networkx
from torch_geometric.data import Data
from util.metrics import eval_admissibility, eval_f1_score

""" Module containing methods originall used in thesis inference experiments. """


def pyg_graph_diameter(x: torch.tensor, edge_index: torch.tensor) -> int:
  G = to_networkx(Data(x=x, edge_index=edge_index)).to_undirected()
  
  diameter = max(nx.diameter(G.subgraph(comp)) for comp in nx.connected_components(G))
  
  return diameter


def graph_density(n: int, e: int, directed: bool) -> float:
  if n == 0 or n == 1:
    return 0
  d = float(e) / float(n * (n - 1))
  if not directed:
    d *= 2
  return d


def print_quartile_desc(desc):
  print("{0:<20} {1:>10} {2:>10} {3:>10} {4:>10} {5:>10}".format(desc, "Q1", "median", "Q3", "min", "max"))
  return


def get_quartiles(data):
  q1 = np.percentile(data, 25)
  q2 = np.percentile(data, 50)
  q3 = np.percentile(data, 75)
  return q1, q2, q3


def print_quartiles(desc: str, data: np.array, floats: bool = False):
  q1, q2, q3 = get_quartiles(data)
  if floats:
    print(f"{desc:<20} {q1:>10.3f} {q2:>10.3f} {q3:>10.3f} {min(data):>10.3f} {max(data):>10.3f}")
  else:
    data = np.round(data).astype(int)
    print(f"{desc:<20} {q1:>10} {q2:>10} {q3:>10} {min(data):>10} {max(data):>10}")


def get_y_stats(dataset):
  ys = []
  for data in dataset:
    y = round(data.y)
    ys.append(y)

  ys = np.array(ys)
  # os.makedirs("plots/", exist_ok=True)
  # plt.hist(ys, bins=round(np.max(ys) + 1),
  #          range=(0, round(np.max(ys) + 1)))
  # plt.xlim(left=0)
  # # plt.title('y distribution')
  # plt.savefig('plots/y_distribution.pdf', bbox_inches="tight")
  # plt.clf()

  return ys


def get_stats(dataset, iteration_stats=False, desc=""):
  if len(dataset) == 0:
    return
  cnt = {}
  max_cost = 0
  graph_nodes = []
  graph_edges = []
  graph_dense = []
  iterations = []

  for data in dataset:
    y = data.y
    if y not in cnt:
      cnt[y] = 0
    cnt[y] += 1
    max_cost = max(max_cost, round(y))

    if iteration_stats:
      iterations.append(data.iterations)

    if data.x is None:
      n_nodes = 0
    else:
      n_nodes = data.x.shape[0]
    try:
      n_edges = data.edge_index.shape[1]
    except:
      # print(data.edge_index)
      # for a in data.edge_index:
      #   print(a)
      n_edges = sum(e.shape[1] for e in data.edge_index)

    density = graph_density(n_nodes, n_edges, directed=True)
    graph_nodes.append(n_nodes)
    graph_edges.append(n_edges)
    graph_dense.append(density)

  # Cost/y distribution
  # print('Cost distribution')
  ys = get_y_stats(dataset)

  # Statistics
  print_quartile_desc(desc)
  if iteration_stats:
    print_quartiles("iterations:", iterations)
  print_quartiles("costs:", ys)
  print_quartiles("n_nodes:", graph_nodes)
  print_quartiles("n_edges:", graph_edges)
  print_quartiles("density:", graph_dense, floats=True)

  return


def view_confusion_matrix(plt_title, y_pred, y_true, view_cm, alt_save="", cutoff=-1, fontsize=None, removeaxeslabel=False):
  if fontsize is not None:
    plt.rcParams.update({'font.size': fontsize})
  y_pred = [round(i) for i in y_pred]
  y_true = [round(i) for i in y_true]
  # min_true = min(y_true)
  # y_pred = y_pred + list(range(min_true))
  # y_true = y_true + list(range(min_true))
  fig, ax = plt.subplots(figsize=(10, 10))
  if cutoff == -1:
    cutoff = max(y_true)+1
  cm = confusion_matrix(y_true, y_pred, normalize="true", labels=list(range(0,cutoff)))
  display_labels = None
  max_y = cm.shape[0]
  if max_y >= 50:
    display_labels = []
    for y in range(max_y):
      if y % 10 == 0:
        display_labels.append(y)
      else:
        display_labels.append("")
  disp = ConfusionMatrixDisplay(confusion_matrix=cm, display_labels=display_labels)
  disp.plot(include_values=False, xticks_rotation="vertical", ax=ax, colorbar=False, cmap=plt.cm.Blues)
  disp.im_.set_clim(0, 1)
  plt_title = str(plt_title)
  # plt.title(plt_title)
  plt_title = ' '.join(plt_title.split())
  plt_title = plt_title.replace(" ", "_")
  plt.axis("off")
  if removeaxeslabel:
    plt.gca().xaxis.label.set_visible(False)
    plt.gca().yaxis.label.set_visible(False)
  if alt_save != "":
    # alt_save = alt_save.replace(".pdf", "")
    # alt_save = alt_save.replace(".png", "")
    plt.savefig(f"{alt_save}", bbox_inches="tight")
  else:
    plt_title = plt_title.replace(".pdf", "")
    plt_title = plt_title.replace(".png", "")
    plt.savefig(f"plots/{plt_title}.pdf")
  if view_cm:
    print(f"Showing {plt_title}")
    plt.show()
  plt.clf()
  return


@torch.no_grad()
def visualise_loader_stats(model, device, loader, title):
  # visualise_train_stats so disgusting so just make another one here
  model.eval()
  y_true = torch.tensor([])
  y_pred = torch.tensor([])
  for data in tqdm(loader):
    data = data.to(device)
    y = data.y
    out = model.forward(data)

    y_pred = torch.cat((y_pred, out.detach().cpu()))
    y_true = torch.cat((y_true, y.detach().cpu()))

  loss = torch.nn.MSELoss()(y_pred, y_true)
  macro_f1, micro_f1 = eval_f1_score(y_pred=y_pred, y_true=y_true)
  admis = eval_admissibility(y_pred=y_pred, y_true=y_true)
  print(f"size: {len(y_true)}")
  print(f"loss: {loss:.2f}")
  print(f"f1: {macro_f1:.1f}")
  print(f"admissibility: {admis:.1f}")
  title = f"{title} f1={macro_f1:.1f} loss={loss:.2f}"
  view_confusion_matrix(title, y_pred.tolist(), y_true.tolist(), view_cm=True)
  return


@torch.no_grad()
def visualise_train_stats(model, device, train_loader, val_loader=None, test_loader=None, max_cost=20, print_stats=True,
                          classify=False, view_cm=False, cm_train="cm_train", cm_val="cm_val", cm_test="cm_test"):
  model = model.to(device)
  model.eval()

  def get_stats_from_loader(loader):
    preds = []
    true = []
    errors = [[] for _ in range(max_cost+1)]
    for batch in tqdm(loader):
      batch = batch.to(device)
      y = batch.y
      out = model.forward(batch)
      if classify:
        out = torch.argmax(out, dim=1)
      else:
        out = torch.maximum(out, torch.zeros_like(out))  # so h is nonzero
      batch_errors = (y - out) / y
      for i in range(len(y)):
        e = batch_errors[i].detach().cpu().item()
        c = y[i].detach().cpu().item()
        o = out[i].detach().cpu().item()
        preds.append(round(o))
        true.append(c)
        errors[0].append(e)
        if c <= max_cost:
          errors[round(c)].append(c - o)
    errors[0] = np.array(errors[0])
    errors[0][np.isnan(errors[0])] = 0
    preds = np.array(preds)
    true = np.array(true)
    return preds, true, errors

  print("Collecting stats...")

  # print("Prediction value set", np.unique(train_preds, return_counts=True))
  os.makedirs("plots", exist_ok=True)
  for fname in ["error_prop", "preds_train", "error_train", "preds_val", "error_val", "preds_test", "error_test"]:
    try:
      os.remove(f"plots/{fname}.png")
    except:
      pass

  boxes = []
  ticks = []

  if train_loader is not None:
    train_preds, train_true, train_errors = get_stats_from_loader(train_loader)
    view_confusion_matrix(plt_title=cm_train, y_true=train_true, y_pred=train_preds, view_cm=view_cm)
    # boxes.append(train_errors[0])
    # ticks.append((len(boxes), 'train'))
    # plt.hist(train_preds, bins=round(np.max(train_preds) + 1),
    #          range=(0, round(np.max(train_preds) + 1)))
    # plt.title('Train prediction distribution')
    # plt.savefig('plots/preds_train', dpi=480)
    # plt.clf()
    #
    # plt.boxplot([train_errors[i] for i in range(1, max_cost + 1)])
    # plt.title('Train error differences over states away from target')
    # plt.ylim((-4, 4))
    # plt.tight_layout()
    # plt.savefig('plots/error_train', dpi=480)
    # plt.clf()
  if val_loader is not None:
    val_preds, val_true, val_errors = get_stats_from_loader(val_loader)
    view_confusion_matrix(plt_title=cm_val, y_true=val_true, y_pred=val_preds, view_cm=view_cm)
    # boxes.append(val_errors[0])
    # ticks.append((len(boxes), 'val'))
    # plt.hist(val_preds, bins=round(np.max(val_preds) + 1),
    #          range=(0, round(np.max(val_preds) + 1)))
    # plt.title('Validation prediction distribution')
    # plt.savefig('plots/preds_val', dpi=480)
    # plt.clf()
    #
    # plt.boxplot([val_errors[i] for i in range(1, max_cost + 1)])
    # plt.title('Val error differences over states away from target')
    # plt.ylim((-4, 4))
    # plt.tight_layout()
    # plt.savefig('plots/error_val', dpi=480)
    # plt.clf()
  if test_loader is not None:
    test_preds, test_true, test_errors = get_stats_from_loader(test_loader)
    view_confusion_matrix(plt_title=cm_test, y_true=test_true, y_pred=test_preds, view_cm=view_cm)
    # boxes.append(test_errors[0])
    # ticks.append((len(boxes), 'test'))
    # plt.hist(test_preds, bins=round(np.max(test_preds) + 1),
    #          range=(0, round(np.max(test_preds) + 1)))
    # plt.title('Test prediction distribution')
    # plt.savefig('plots/preds_val', dpi=480)
    # plt.clf()
    #
    # plt.boxplot([test_errors[i] for i in range(1, max_cost + 1)])
    # plt.title('Test error differences over states away from target')
    # plt.ylim((-4, 4))
    # plt.tight_layout()
    # plt.savefig('plots/error_test', dpi=480)
    # plt.clf()

  print("Plotting done!")

  # Statistics
  if print_stats:
    print("{0:<20} {1:>10} {2:>10} {3:>10} {4:>10} {5:>10}".format(" ", "Q1", "median", "Q3", "min", "max"))
    if train_loader is not None:
      print_quartiles("train prop_err:", train_errors[0], floats=True)
    if val_loader is not None:
      print_quartiles("val prop_err:", val_errors[0], floats=True)
    if test_loader is not None:
      print_quartiles("test prop_err:", test_errors[0], floats=True)
    print("% admissible")
    if train_loader is not None:
      print(f"train: {np.count_nonzero(train_errors[0] > 0) / len(train_errors[0]):.2f}")
    if val_loader is not None:
      print(f"val: {np.count_nonzero(val_errors[0] > 0) / len(val_errors[0]):.2f}")
    if test_loader is not None:
      print(f"test: {np.count_nonzero(test_errors[0] > 0) / len(test_errors[0])}:.2f")

  # plt.boxplot(boxes)
  # plt.xticks(ticks)
  # plt.ylim((-1, 1))
  # plt.title('Proportion errors')
  # plt.tight_layout()
  # plt.savefig('plots/error_prop', dpi=480)
  # plt.clf()

  return
