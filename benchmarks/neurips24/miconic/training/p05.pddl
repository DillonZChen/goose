(define (problem miconic_05-problem)
 (:domain miconic_05-domain)
 (:objects
   p1 p2 - passenger
   f1 f2 - floor
 )
 (:init (lift-at f2) (origin p1 f2) (origin p2 f1) (destin p1 f1) (destin p2 f2) (above f1 f2) (= (lift-capacity) 4) (= (weight p1) 2) (= (weight p2) 2))
 (:goal (and (served p1) (served p2)))
)
