from .base_gnn import *

class MPNNLayer(Module):
    def __init__(self, in_features: int, out_features: int):
      super(MPNNLayer, self).__init__()
      self.conv = LinearMaxConv(in_features, out_features)
      # self.conv = LinearMaxConv(in_features, out_features).jittable()
      self.linear = Linear(in_features, out_features)

    def forward(self, x: Tensor, edge_index: Tensor) -> Tensor:
      x_out = self.linear(x) + self.conv(x, edge_index)
      return x_out
    

""" simple MPNN with linear updates """
class MPNN(BaseGNN):
  def __init__(self, params) -> None:
    super().__init__(params)
    if self.drop > 0:
      warnings.warn("dropout not implemented for MPNN")
    return

  def create_layer(self):
    return MPNNLayer(self.nhid, self.nhid)
  

class MPNNPredictor(BasePredictor):
  def __init__(self, params, jit=True) -> None:
    super().__init__(params, jit)
    return

  def create_model(self, params):
    self.model = MPNN(params)
    return
  