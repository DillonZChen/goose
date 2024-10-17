(define (problem miconic_03-problem)
 (:domain miconic_03-domain)
 (:objects
   p1 p2 - passenger
   f1 f2 - floor
 )
 (:init (lift-at f1) (origin p1 f1) (origin p2 f1) (destin p1 f2) (destin p2 f2) (above f1 f2) (= (lift-capacity) 4) (= (weight p1) 1) (= (weight p2) 2))
 (:goal (and (served p1) (served p2)))
)
