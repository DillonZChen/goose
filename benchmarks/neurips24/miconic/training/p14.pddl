(define (problem miconic_14-problem)
 (:domain miconic_14-domain)
 (:objects
   p1 p2 - passenger
   f1 f2 f3 - floor
 )
 (:init (lift-at f3) (origin p1 f1) (destin p1 f2) (origin p2 f3) (destin p2 f1) (above f1 f2) (above f1 f3) (above f2 f3) (= (lift-capacity) 4) (= (weight p1) 3) (= (weight p2) 3))
 (:goal (and (served p1) (served p2)))
)
