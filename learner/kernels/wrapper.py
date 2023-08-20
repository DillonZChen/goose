import numpy as np
import kernels
from planning import Proposition, State
from sklearn.svm import LinearSVR, SVR
from typing import List
from representation import CGraph, Representation, REPRESENTATIONS


_MAX_MODEL_ITER = 10000

class KernelModelWrapper():  # TODO optimise memory
  def __init__(self, args) -> None:
    super().__init__()
    self._model_name = args.model

    self._kernel = kernels.KERNELS[args.kernel](
      iterations=args.iterations,
      all_colours=args.all_colours,
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
  
  def lifted_state_input(self) -> bool:
    return self._representation.lifted
    
  def update_representation(self, domain_pddl: str, problem_pddl: str):
    self._representation : Representation = REPRESENTATIONS[self._rep_type](domain_pddl, problem_pddl)
    self._representation.convert_to_coloured_graph()
    return
    
  def fit(self, X, y) -> None:
    self._model.fit(X, y)
    
  def get_learning_model(self):
    return self._model
    
  def read_train_data(self, graphs: CGraph) -> None:
    self._kernel.read_train_data(graphs)

  def get_matrix_representation(self, graphs: CGraph) -> np.array:
    if self._model_name == "linear-svr":
      return self._kernel.get_x(graphs)
    elif self._model_name == "svr":
      return self._kernel.get_k(graphs)
    else:
      raise ValueError(self._model_name)

  def h(self, state: State) -> float:
    # TODO
    raise NotImplementedError
    return 0

  def h_batch(self, states: List[State]) -> List[float]:
    # TODO
    raise NotImplementedError
    return [0 for _ in states]
  