(define (problem miconic-01)
 (:domain miconic)
 (:objects
    p1 - passenger
    f1 f2 - floor
    )
 (:init
    (lift-at f1)
    (origin p1 f2)
    (destin p1 f1)
    (above f1 f2)
)
 (:goal  (and (served p1))))
