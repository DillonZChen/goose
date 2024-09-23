# Available at setup time due to pyproject.toml
from glob import glob

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

# Read version from wlplan/__version__.py file
exec(open("wlplan/__version__.py").read())

# Sort input source files if you glob sources to ensure bit-for-bit
# reproducible builds (https://github.com/pybind/python_example/pull/53)
files = [glob("src/*.cpp"), glob("src/**/*.cpp")]

ext_modules = [
    Pybind11Extension(
        "_wlplan",
        sorted([f for file_group in files for f in file_group]),
        # Example: passing in the version to the compiled code
        define_macros=[("WLPLAN_VERSION", __version__)],
    ),
]

setup(
    name="wlplan",
    version=__version__,
    author="Dillon Z. Chen",
    author_email="dillon.chen1@gmail.com",
    description="WLPlan: Relational Features for PDDL Planning",
    long_description=open("README.md").read(),
    long_description_content_type="text/markdown",
    packages=["wlplan", "_wlplan"],
    package_data={"_wlplan": ["py.typed", "*.pyi", "**/*.pyi"]},
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    project_urls={"GitHub": "https://github.com/DillonZChen/wlplan"},
    license="MIT License",
    python_requires=">=3.10",
    install_requires=[
        "pymdzcf==0.1.0",
        "networkx>=3.0",
    ],
)
