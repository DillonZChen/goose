from learning.predictor.linear_model.mixed_integer_program import MixedIntegerProgram


class LinearProgramRanker(MixedIntegerProgram):
    """Ranking with MIP formulation"""

    IS_RANK = True
