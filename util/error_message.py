import os


def get_path_error_msg(path):
    pwd = os.getcwd()
    ls = os.listdir(pwd)
    # return f"Path {path} does not exist. Current directory is `{pwd}` and contains:\n{ls}"
    return f"Path {path} does not exist. The current directory is {pwd}."
