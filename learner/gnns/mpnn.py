from .base_gnn import *

class MPNNLayer(Module):
    def __init__(self, in_features: int, out_features: int, aggr: str):
      super(MPNNLayer, self).__init__()
      self.conv = LinearConv(in_features, out_features, aggr=aggr)
      self.root = Linear(in_features, out_features)

    def forward(self, x: Tensor, edge_index: Tensor) -> Tensor:
      x_out = self.root(x) + self.conv(x, edge_index)
      return x_out
    

""" Simple MPNN with linear updates. Does not support edge labeled graphs. """
class MPNN(BaseGNN):
  def __init__(self, params) -> None:
    super().__init__(params)
    if self.share_layers:
      raise NotImplementedError("sharing layers not implemented for MPNN")
    return

  def create_layer(self):
    return MPNNLayer(self.nhid, self.nhid, aggr=self.aggr)
  

class MPNNPredictor(BasePredictor):
  def __init__(self, params, jit=True) -> None:
    super().__init__(params, jit)
    return

  def create_model(self, params):
    self.model = MPNN(params)
    return
  