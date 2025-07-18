from goose.learning.predictor.linear_model.rank_classifier import RankClassifier


class SupportVectorMachineRanker(RankClassifier):
    """Ranking with SVM formulation"""

    IS_RANK = True
