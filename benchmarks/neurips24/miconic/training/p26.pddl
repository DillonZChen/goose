(define (problem miconic_26-problem)
 (:domain miconic_26-domain)
 (:objects
   p1 p2 - passenger
   f1 f2 f3 f4 f5 f6 - floor
 )
 (:init (lift-at f5) (origin p1 f2) (destin p1 f5) (origin p2 f5) (destin p2 f4) (above f1 f2) (above f1 f3) (above f1 f4) (above f1 f5) (above f1 f6) (above f2 f3) (above f2 f4) (above f2 f5) (above f2 f6) (above f3 f4) (above f3 f5) (above f3 f6) (above f4 f5) (above f4 f6) (above f5 f6) (= (lift-capacity) 4) (= (weight p1) 3) (= (weight p2) 3))
 (:goal (and (served p1) (served p2)))
)
