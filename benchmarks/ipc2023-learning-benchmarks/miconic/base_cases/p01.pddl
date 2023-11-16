(define (problem miconic-01)
 (:domain miconic)
 (:objects 
    p1 - passenger
    f1 f2 - floor
    )
 (:init 
    (lift-at f2)
    (origin p1 f1)
    (destin p1 f2)
    (above f1 f2)
)
 (:goal  (and (served p1))))
