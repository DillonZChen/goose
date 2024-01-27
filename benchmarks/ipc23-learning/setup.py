from setuptools import setup, find_packages


def main():
    setup(
        name="IPC23 - Learning Track",
        version="0.0.1",
        author="Javier Segovia-Aguas and Jendrik Seipp",
        packages=find_packages("."),
        package_dir={"": "."},
        setup_requires=["wheel"],
    )


if __name__ == "__main__":
    main()
