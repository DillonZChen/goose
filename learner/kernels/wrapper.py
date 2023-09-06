import numpy as np
import kernels
from sklearn.linear_model import Lasso, Ridge, LinearRegression
from sklearn.svm import LinearSVR, SVR
from typing import List, Optional, Dict
from representation import CGraph, Representation, REPRESENTATIONS
from planning import State
from kernels.base_kernel import Histogram


MODELS = [
  "linear-regression",
  "linear-svr",
  "ridge",
  "lasso",

  "rbf-svr",
  "quadratic-svr",
  "cubic-svr",
]

_MAX_MODEL_ITER = 10000

class KernelModelWrapper():
  def __init__(self, args) -> None:
    super().__init__()
    self._model_name = args.model

    self._kernel = kernels.KERNELS[args.kernel](
      iterations=args.iterations,
    )

    self._rep_type = args.rep
    self._representation = None

    kwargs = {
      "max_iter": _MAX_MODEL_ITER,
    }
    self._model = {
      "linear-regression": LinearRegression(),
      "linear-svr": LinearSVR(dual="auto", epsilon=args.e, C=args.C, max_iter=_MAX_MODEL_ITER),
      "lasso": Lasso(alpha=args.a, max_iter=_MAX_MODEL_ITER),
      "ridge": Ridge(alpha=args.a, max_iter=_MAX_MODEL_ITER),

      "rbf-svr": SVR(kernel="rbf", epsilon=args.e, C=args.C, max_iter=_MAX_MODEL_ITER),
      "quadratic-svr": SVR(kernel="poly", degree=2, epsilon=args.e, C=args.C, max_iter=_MAX_MODEL_ITER),
      "cubic-svr": SVR(kernel="poly", degree=3, epsilon=args.e, C=args.C, max_iter=_MAX_MODEL_ITER),
    }[self._model_name]

    self._train = True
    self._indices = None
    
  def train(self) -> None:
    self._kernel.train()
  
  def eval(self) -> None:
    self._kernel.eval()
    
  def fit(self, X, y) -> None:
    self._model.fit(X, y)
    
  def predict(self, X) -> np.array:
    return self._model.predict(X)
    
  def get_learning_model(self):
    return self._model
  
  def lifted_state_input(self) -> bool:
    return self._representation.lifted
    
  def update_representation(self, domain_pddl: str, problem_pddl: str) -> None:
    self._representation : Representation = REPRESENTATIONS[self._rep_type](domain_pddl, problem_pddl)
    self._representation.convert_to_coloured_graph()
    return
  
  def get_iterations(self) -> int:
    return self._kernel.iterations
  
  def get_weight_indices(self):
    """ Boolean array that is the size of self._model.coef_ """
    if hasattr(self, "_indices") and self._indices is not None:
      return self._indices
    return np.ones_like(self.get_weights())
  
  def set_weight_indices(self, indices):
    self._indices = indices
    return

  def get_weights(self):
    weights = self._model.coef_
    if hasattr(self, "_indices") and self._indices is not None:
      weights = weights[self._indices]
    return weights
  
  def get_bias(self) -> float:
    bias = self._model.intercept_
    if type(bias) == float:
      return bias
    if type(bias) == np.float64:
      return float(bias)
    return float(bias[0])  # linear-svr returns array
  
  def get_num_weights(self):
    return len(self.get_weights())
  
  def get_num_zero_weights(self):
    return np.count_nonzero(self.get_weights()==0)
  
  def write_model_data(self) -> None:
    from datetime import datetime
    df = self._representation.domain_pddl
    pf = self._representation.problem_pddl
    t = datetime.now().strftime('%Y-%m-%d-%H-%M-%S')
    file_path = "_".join(["graph", df, pf, t])
    file_path = file_path.replace("/","-").replace(".pddl","").replace(".","")
    file_path = file_path + ".model"

    model_hash = self.get_hash()
    indices = self.get_weight_indices()
    weights = self.get_weights()
    bias = self.get_bias()
    iterations = self.get_iterations()

    zero_weights = np.count_nonzero(weights==0)
    print(f"{zero_weights}/{len(weights)} = {zero_weights/len(weights):.2f} are zero")

    # prune zero weights
    new_weights = []
    new_model_hash = {}

    reverse_hash = {model_hash[k]: k for k in model_hash}

    for colour, weight in enumerate(weights):
      if abs(weight) == 0 or indices[colour] == 0:
        continue

      new_weights.append(weight)

      key = reverse_hash[colour]
      val = model_hash[key]
      new_model_hash[key] = val

    model_hash = new_model_hash
    weight = new_weights

    # write data
    with open(file_path, 'w') as f:
      f.write(f"{len(model_hash)} hash size\n")
      for k in model_hash:
        f.write(f"{k} {model_hash[k]}\n")
      f.write(f"{len(weights)} weights size\n")
      for weight in weights:
        f.write(str(weight) + '\n')
      f.write(f"{bias} bias\n")
      f.write(f"{iterations} iterations\n")
      f.close()

    self._model_data_path = file_path
    pass

  def get_model_data_path(self) -> str:
    return self._model_data_path
  
  def write_representation_to_file(self) -> None:
    self._representation.write_to_file()
    return
  
  def get_graph_file_path(self) -> str:
    return self._representation.get_graph_file_path()
  
  def get_hash(self) -> Dict[str, int]:
    return self._kernel.get_hash()
    
  def compute_histograms(self, graphs: CGraph) -> None:
    return self._kernel.compute_histograms(graphs)

  def get_matrix_representation(
    self, 
    graphs: CGraph, 
    histograms: Optional[Dict[CGraph, Histogram]]
  ) -> np.array:
    if self._model_name == "svr":
      return self._kernel.get_k(graphs, histograms)
    else:
      return self._kernel.get_x(graphs, histograms)

  def h(self, state: State) -> float:
    h = self.h_batch([state])[0]
    return h

  def h_batch(self, states: List[State]) -> List[float]:
    graphs = [self._representation.state_to_cgraph(state) for state in states]
    X = self._kernel.get_x(graphs)
    y = self.predict(X)
    hs = np.rint(y).astype(int).tolist()
    return hs
  
  @property
  def n_colours_(self) -> int:
    return self._kernel.n_colours_
  