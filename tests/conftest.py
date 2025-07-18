import sys
from pathlib import Path

import pytest


root = Path(__file__).parent.parent
sys.path.insert(0, str(root))
sys.path.insert(0, str(root / "goose"))
sys.path.insert(0, str(root / "goose" / "util"))
sys.path.insert(0, str(root / "goose" / "enums"))


def pytest_addoption(parser):
    """Add custom command line options to pytest"""

    parser.addoption("--apptainer", action="store_true", help="Test on built apptainer image")


@pytest.fixture(scope="session")
def apptainer(request):
    return request.config.getoption("--apptainer")
