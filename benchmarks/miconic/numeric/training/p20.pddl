(define (problem miconic_20-problem)
 (:domain miconic_20-domain)
 (:objects
   p1 - passenger
   f1 f2 f3 f4 f5 - floor
 )
 (:init (lift-at f3) (origin p1 f1) (destin p1 f5) (above f1 f2) (above f1 f3) (above f1 f4) (above f1 f5) (above f2 f3) (above f2 f4) (above f2 f5) (above f3 f4) (above f3 f5) (above f4 f5) (= (lift-capacity) 4) (= (weight p1) 3))
 (:goal (and (served p1)))
)
