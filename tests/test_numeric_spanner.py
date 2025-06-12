import pytest
from functions import plan, train

BENCHMARKS = "neurips24"
DOMAIN = "spanner"
PREDICTOR = "ccwl/ccwl_rank-lp_1"
PROBLEM = "2_30"
EXPECTED_EXPANDED_UB = None


@pytest.mark.parametrize("domain", [DOMAIN])
def test_domain(domain):
    model_path = f"tests/models/{domain}.model"
    train(
        domain=domain,
        save_path=model_path,
        predictor=PREDICTOR,
        benchmarks=BENCHMARKS,
    )
    plan(
        domain=domain,
        problem=PROBLEM,
        evaluator=model_path,
        planner="nfd",
        expected_expanded_ub=EXPECTED_EXPANDED_UB,
        benchmarks=BENCHMARKS,
    )


if __name__ == "__main__":
    test_domain()
