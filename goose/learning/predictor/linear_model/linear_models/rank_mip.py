from goose.learning.predictor.linear_model.rank_linear_program import RankLinearProgram


class MixedIntegerProgramRanker(RankLinearProgram):
    """Ranking with MIP formulation"""

    IS_RANK = True
