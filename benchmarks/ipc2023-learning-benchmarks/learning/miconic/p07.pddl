(define (problem miconic-07)
 (:domain miconic)
 (:objects
    p1 - passenger
    f1 f2 f3 - floor
    )
 (:init
    (lift-at f2)
    (origin p1 f3)
    (destin p1 f1)
    (above f1 f2)
    (above f1 f3)
    (above f2 f3)
)
 (:goal  (and (served p1))))
