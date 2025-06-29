import pytest


def pytest_addoption(parser):
    """Add custom command line options to pytest"""

    parser.addoption("--apptainer", action="store_true", help="Test on built apptainer image")


@pytest.fixture(scope="session")
def apptainer(request):
    return request.config.getoption("--apptainer")
