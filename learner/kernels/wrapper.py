import numpy as np
import kernels
from sklearn.svm import LinearSVR, SVR
from typing import List, Optional, Dict
from representation import CGraph, Representation, REPRESENTATIONS
from planning import State
from kernels.base_kernel import Histogram


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
      "epsilon": args.e,
      "C": args.C,
      "max_iter": _MAX_MODEL_ITER,
    }
    if self._model_name == "linear-svr":
      self._model = LinearSVR(dual="auto", **kwargs)
    elif self._model_name == "svr":
      self._model = SVR(kernel="precomputed", **kwargs)
    else:
      raise NotImplementedError

    self._train = True
    
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

  def get_weights(self):
    return self._model.coef_
  
  def get_bias(self):
    return self._model.intercept_
  
  def write_model_data(self) -> None:
    from datetime import datetime
    df = self._representation.domain_pddl
    pf = self._representation.problem_pddl
    t = datetime.now().strftime('%Y-%m-%d-%H-%M-%S')
    file_path = "_".join(["graph", df, pf, t])
    file_path = file_path.replace("/","-").replace(".pddl","").replace(".","")
    file_path = file_path + ".model"

    model_hash = self.get_hash()
    weights = self.get_weights()
    bias = self.get_bias()
    iterations = self.get_iterations()

    with open(file_path, 'w') as f:
      f.write(f"{len(model_hash)} hash size\n")
      for k in model_hash:
        f.write(f"{k} {model_hash[k]}\n")
      f.write(f"{len(weights)} weights size\n")
      for weight in weights:
        f.write(str(weight) + '\n')
      f.write(f"{bias[0]} bias\n")
      f.write(f"{iterations} iterations\n")
      f.close()

    zero_weights = np.count_nonzero(weights==0)
    print(f"{zero_weights}/{len(weights)} = {zero_weights/len(weights):.2f}% are zero")

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
    if self._model_name == "linear-svr":
      return self._kernel.get_x(graphs, histograms)
    elif self._model_name == "svr":
      return self._kernel.get_k(graphs, histograms)
    else:
      raise ValueError(self._model_name)

  def h(self, state: State) -> float:
    h = self.h_batch([state])[0]
    return h

  def h_batch(self, states: List[State]) -> List[float]:
    graphs = [self._representation.state_to_cgraph(state) for state in states]

    X = self._kernel.get_x(graphs)
    y = self.predict(X)
    hs = np.rint(y).astype(int).tolist()
    return hs

    # if len(states)==0:
    #   graphs = [self._representation.state_to_cgraph(state) for state in states]
    #   X = self._kernel.get_x(graphs)
    #   y = self.predict(X)
    #   hs = np.rint(y).astype(int).tolist()
    #   return hs
    # import time 
    # rep = 0
    # x = 0
    # pred = 0
    # for _ in range(100):
    #   t = time.time()
    #   graphs = [self._representation.state_to_cgraph(state) for state in states]
    #   rep += time.time() - t

    #   t = time.time()
    #   X = self._kernel.get_x(graphs)
    #   x += time.time() - t

    #   t = time.time()
    #   y = self.predict(X)
    #   pred += time.time() - t

    #   hs = np.rint(y).astype(int).tolist()

    # print(" rep:", rep)
    # print("   x:", x)
    # print("pred:", pred)
    # assert 0
    # return hs
  
  @property
  def n_colours_(self) -> int:
    return self._kernel.n_colours_
  