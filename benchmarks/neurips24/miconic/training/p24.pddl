(define (problem miconic_24-problem)
 (:domain miconic_24-domain)
 (:objects
   p1 p2 - passenger
   f1 f2 f3 f4 f5 - floor
 )
 (:init (lift-at f2) (origin p1 f5) (destin p1 f2) (origin p2 f2) (destin p2 f3) (above f1 f2) (above f1 f3) (above f1 f4) (above f1 f5) (above f2 f3) (above f2 f4) (above f2 f5) (above f3 f4) (above f3 f5) (above f4 f5) (= (lift-capacity) 4) (= (weight p1) 1) (= (weight p2) 2))
 (:goal (and (served p1) (served p2)))
)
