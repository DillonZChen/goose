(define (problem miconic_17-problem)
 (:domain miconic_17-domain)
 (:objects
   p1 - passenger
   f1 f2 f3 f4 - floor
 )
 (:init (lift-at f4) (origin p1 f1) (destin p1 f2) (above f1 f2) (above f1 f3) (above f1 f4) (above f2 f3) (above f2 f4) (above f3 f4) (= (lift-capacity) 4) (= (weight p1) 1))
 (:goal (and (served p1)))
)
