import os


def extract_problems(prob_dir: str, file_Name, i):
    """ Generate problems using http://users.cecs.anu.edu.au/~jks/cgi-bin/bwstates/bwcgi """
    prev_line = "\n"

    problems = []
    cur_problem = []

    with open(os.path.join(prob_dir, file_Name), "r") as f:
        for line in f:
            if line == "\n" and not prev_line == "\n":
                problems.append(
                    [line.replace("\n", "") for line in cur_problem if line != "\n"]
                )
                cur_problem = []
                prev_line = "\n"
            elif line == "\n":
                continue
            else:
                if ":init" in prev_line:
                    cur_problem.append("        (handempty)")
                if ":objects" in line:
                    line = line.replace(")", " - block)")

                cur_problem.append(line)
                prev_line = line

    cur_problem = [line.replace("\n", "") for line in cur_problem if line != "\n"]
    if len(cur_problem) > 0:
        problems.append(cur_problem)

    for idx, problem in enumerate(problems):
        f_name = os.path.join(prob_dir, f"blocks{i}-task{'0' if idx + 1 < 10 else ''}{idx + 1}-seed9000.pddl")
        with open(f_name, "w") as f:
            f.write("\n".join(problem))
            print(f"Wrote problem to {f_name}")


if __name__ == "__main__":
    # import sys

    # for dir_ in sys.argv[1:]:
    #     extract_problems(dir_)

    for i in range(3,11):
        extract_problems(f"test_small/", f"{i}.pddl", i)

