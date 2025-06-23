import os

""" Tests whether code does not crash """


def test_distinguishability_test():
    cmd = "./goose.sif train configurations/data/ipc23lt/blocksworld.toml configurations/model/classic.toml --distinguish_test"
    rc = os.system(cmd)
    assert rc == 0


if __name__ == "__main__":
    test_distinguishability_test()
