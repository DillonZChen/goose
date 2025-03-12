import pytest
from functions import plan, train

BENCHMARKS = "neurips24"
DOMAIN = "childsnack"
PREDICTOR = "ccwl/ccwl_rank-lp_1"
PROBLEM = "2_30"
EXPECTED_EXPANDED_UB = None


@pytest.mark.parametrize("domain", [DOMAIN])
def test_domain(domain):
    modelpath = f"tests/models/{domain}.model"
    train(
        domain,
        modelpath,
        predictor=PREDICTOR,
        benchmarks=BENCHMARKS,
        numeric=True,
    )
    plan(
        domain,
        PROBLEM,
        modelpath,
        "nfd",
        expected_expanded_ub=EXPECTED_EXPANDED_UB,
        benchmarks=BENCHMARKS,
        numeric=True,
    )


if __name__ == "__main__":
    test_domain()
