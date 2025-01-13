import pytest
from functions import plan, train

BENCHMARKS = "ipc23lt"
DOMAIN = "blocksworld"
PREDICTOR = "wl/wl_gpr_4"
PROBLEM = "1_01"
EXPECTED_EXPANDED_UB = None


@pytest.mark.parametrize("domain", [DOMAIN])
def test_domain(domain):
    modelpath = f"tests/models/{domain}.model"
    train(
        domain,
        modelpath,
        predictor=PREDICTOR,
        benchmarks=BENCHMARKS,
        numeric=False,
    )
    plan(
        domain,
        PROBLEM,
        modelpath,
        "fd",
        expected_expanded_ub=EXPECTED_EXPANDED_UB,
        benchmarks=BENCHMARKS,
        numeric=False,
    )


if __name__ == "__main__":
    test_domain()
