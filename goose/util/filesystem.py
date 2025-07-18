import os


def file_exists(file_path: str) -> bool:
    return os.path.exists(file_path) and os.path.isfile(file_path)


def get_path_error_msg(path: str) -> str:
    pwd = os.getcwd()
    # ls = os.listdir(pwd)
    # return f"Path {path} does not exist. Current directory is `{pwd}` and contains:\n{ls}"
    return f"Path {path} does not exist. The current directory is {pwd}."
