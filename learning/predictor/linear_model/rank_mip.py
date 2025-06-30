from learning.predictor.linear_model.mixed_integer_program import MixedIntegerProgram


class MixedIntegerProgramRanker(MixedIntegerProgram):
    """Ranking with MIP formulation"""

    IS_RANK = True
