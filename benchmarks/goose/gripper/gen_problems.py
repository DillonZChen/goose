import os

def gen_prob(i):
    f = open(f"test/gripper-n{i}.pddl", "w")
    f.write(f"(define (problem gripper-{i})")
