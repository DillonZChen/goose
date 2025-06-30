from learning.predictor.linear_model.classifier import Classifier


class GaussianProcessClassifierRanker(Classifier):
    """Linear GPC for Ranking with Laplace Approximation"""

    IS_RANK = True
