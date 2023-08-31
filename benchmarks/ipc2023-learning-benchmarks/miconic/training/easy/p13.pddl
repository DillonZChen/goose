(define (problem miconic-13)
(:domain miconic)
(:objects
    p1 p2 p3 p4 - passenger
    f1 f2 - floor
)
(:init
    (lift-at f1)
    (origin p1 f2)
    (origin p2 f1)
    (origin p3 f2)
    (origin p4 f1)
    (destin p1 f1)
    (destin p2 f2)
    (destin p3 f1)
    (destin p4 f2)
    (above f1 f2)
)
(:goal  (and
    (served p1)
    (served p2)
    (served p3)
    (served p4)
)))
