from glob import glob

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import find_packages, setup


def main():
    # Read version from succgen/__version__.py file
    version_ns = {}
    with open("succgen/__version__.py") as f:
        exec(f.read(), version_ns)
    __version__ = version_ns["__version__"]

    # Sort input source files if you glob sources to ensure bit-for-bit
    # reproducible builds (https://github.com/pybind/python_example/pull/53)
    source_files = sorted([f for pattern in ["src/*.cpp", "src/**/*.cpp", "src/**/**/*.cpp"] for f in glob(pattern)])

    ext_module = Pybind11Extension("_succgen", source_files)

    setup(
        name="succgen",
        version=__version__,
        author="Dillon Z. Chen",
        author_email="dillon.chen1@gmail.com",
        description="Lifted Successor Generator",
        packages=find_packages(include=["succgen", "succgen.*", "_succgen"]),
        package_data={"_succgen": ["py.typed", "*.pyi", "**/*.pyi"]},
        ext_modules=[ext_module],
        cmdclass={"build_ext": build_ext},
        license="MIT License",
        python_requires=">=3.10",
        install_requires=[],
    )


if __name__ == "__main__":
    main()
