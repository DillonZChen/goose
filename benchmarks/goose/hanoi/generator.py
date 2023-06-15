def gen_problem(n):
    # must run in directory where this file exists
    f = open(f"test/hanoi-n{n}.pddl", 'w')
    f.write(f"(define (problem hanoi-{n})\n")
    f.write(f"  (:domain hanoi-domain)\n")
    objs =  f"  (:objects peg1 peg2 peg3 "
    for i in range(1, n+1):
       objs += f"d{i} "
    objs += ")\n"
    f.write(objs)
    f.write("  (:init \n")
    for i in range(1, n+1):
        f.write(f"    (smaller d{i} peg1)(smaller d{i} peg2)(smaller d{i} peg3)\n")
    f.write("\n")

    for i in range(1, n):
        smaller = "    "
        for j in range(i+1, n+1):
            smaller += f"(smaller d{i} d{j})"
        f.write(smaller+"\n")
    f.write("\n")
    f.write("    (clear peg1)(clear peg2)(clear d1)\n")
    f.write("\n")
    ons = "    "
    for i in range(1, n):
        ons += f"(on d{i} d{i+1})"
    ons += f"(on d{n} peg3)\n"
    f.write(ons)
    f.write("  )\n")
    f.write("  (:goal \n")
    goal = "    (and "
    for i in range(1, n):
        goal += f"(on d{i} d{i+1})"
    goal += f"(on d{n} peg1) )\n"
    f.write(goal)
    f.write("  )\n")
    f.write(")\n")


for n in range(10, 101):
  gen_problem(n)
