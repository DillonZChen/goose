(define (problem miconic-04)
(:domain miconic)
(:objects
    p1 p2 - passenger
    f1 f2 - floor
)
(:init
    (lift-at f2)
    (origin p1 f1)
    (origin p2 f2)
    (destin p1 f2)
    (destin p2 f1)
    (above f1 f2)
)
(:goal  (and
    (served p1)
    (served p2)
)))
