from learning.predictor.mixed_integer_program import MixedIntegerProgram


class LinearProgramRanker(MixedIntegerProgram):
    """Ranking with MIP formulation"""

    IS_RANK = True
