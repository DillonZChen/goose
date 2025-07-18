from goose.learning.predictor.linear_model.rank_linear_program import RankLinearProgram


class LinearProgramRanker(RankLinearProgram):
    """Ranking with MIP formulation"""

    IS_RANK = True
