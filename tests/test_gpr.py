import pytest
from functions import plan, train

DOMAINS = ["blocksworld", "childsnack", "ferry", "miconic", "rovers", "satellite", "spanner", "transport"]
PREDICTOR = "gpr_2"
PROBLEM = "0_01"
EXPECTED_EXPANDED_UB = None


@pytest.mark.parametrize("domain", DOMAINS)
def test_domain(domain):
    modelpath = f"tests/models/{domain}.model"
    train(domain, modelpath, predictor=PREDICTOR)
    plan(domain, PROBLEM, modelpath, "fd", expected_expanded_ub=EXPECTED_EXPANDED_UB)


if __name__ == "__main__":
    test_domain()
