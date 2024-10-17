(define (problem miconic_01-problem)
 (:domain miconic_01-domain)
 (:objects
   p1 - passenger
   f1 f2 - floor
 )
 (:init (lift-at f1) (origin p1 f2) (destin p1 f1) (above f1 f2) (= (lift-capacity) 4) (= (weight p1) 2))
 (:goal (and (served p1)))
)
