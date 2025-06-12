from learning.predictor.classifier import Classifier


class GaussianProcessClassifierRanker(Classifier):
    """Linear GPC for Ranking with Laplace Approximation"""

    IS_RANK = True
