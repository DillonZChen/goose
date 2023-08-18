from .base_gnn import *
from torch_geometric.nn.conv import RGCNConv, FastRGCNConv  # (slow and/or mem inefficient)


class ELMPNNLayer(Module):
    def __init__(self, in_features: int, out_features: int, n_edge_labels: int, aggr: str):
      super(ELMPNNLayer, self).__init__()
      self.convs = torch.nn.ModuleList()
      for _ in range(n_edge_labels):
        self.convs.append(LinearConv(in_features, out_features, aggr=aggr).jittable())
      self.root = Linear(in_features, out_features, bias=True)
      return

    def forward(self, x: Tensor, list_of_edge_index: List[Tensor]) -> Tensor:
      x_out = self.root(x)
      for i, conv in enumerate(self.convs):  # bottleneck; difficult to parallelise efficiently
        x_out += conv(x, list_of_edge_index[i])
      return x_out

""" GNN with different weights for different edge labels """
class ELMPNN(BaseGNN):
  def __init__(self, params) -> None:
    super().__init__(params)
    if self.vn:
      raise NotImplementedError("vn not implemented for ELGNN")
    if self.share_layers:
      raise NotImplementedError("sharing layers not implemented for ELGNN")
    return

  def create_layer(self):
    return ELMPNNLayer(self.nhid, self.nhid, n_edge_labels=self.n_edge_labels, aggr=self.aggr)

  def node_embedding(self, x: Tensor, list_of_edge_index: List[Tensor], batch: Optional[Tensor]) -> Tensor:
    """ overwrite typing (same semantics, different typing) for jit """
    x = self.emb(x)
    for layer in self.layers:
      x = layer(x, list_of_edge_index)
      x = F.relu(x)
    return x
  
  def graph_embedding(self, x: Tensor, list_of_edge_index: List[Tensor], batch: Optional[Tensor]) -> Tensor:
    """ overwrite typing (same semantics, different typing) for jit """
    x = self.node_embedding(x, list_of_edge_index, batch)
    x = self.pool(x, batch)
    return x

  def forward(self, x: Tensor, list_of_edge_index: List[Tensor], batch: Optional[Tensor]) -> Tensor:
    """ overwrite typing (same semantics, different typing) for jit """
    x = self.graph_embedding(x, list_of_edge_index, batch)
    x = self.mlp(x)
    x = x.squeeze(1)
    return x
  

class ELMPNNPredictor(BasePredictor):
  def __init__(self, params, jit=False) -> None:
    super().__init__(params, jit)
    return
  
  def create_model(self, params):
    self.model = ELMPNN(params)

  def h(self, state: State) -> float:
    x, edge_index = self.rep.state_to_tensor(state)
    x = x.to(self.device)
    for i in range(len(edge_index)):
      edge_index[i] = edge_index[i].to(self.device)
    h = self.model.forward(x, edge_index, None).item()
    h = round(h)
    return h

  def predict_action(self, state: State):
    raise NotImplementedError
  