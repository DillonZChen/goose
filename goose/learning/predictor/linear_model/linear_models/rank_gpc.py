from goose.learning.predictor.linear_model.rank_classifier import RankClassifier


class GaussianProcessClassifierRanker(RankClassifier):
    """Linear GPC for Ranking with Laplace Approximation"""

    IS_RANK = True
