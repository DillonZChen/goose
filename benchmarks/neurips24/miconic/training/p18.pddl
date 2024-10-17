(define (problem miconic_18-problem)
 (:domain miconic_18-domain)
 (:objects
   p1 - passenger
   f1 f2 f3 f4 - floor
 )
 (:init (lift-at f3) (origin p1 f4) (destin p1 f2) (above f1 f2) (above f1 f3) (above f1 f4) (above f2 f3) (above f2 f4) (above f3 f4) (= (lift-capacity) 4) (= (weight p1) 3))
 (:goal (and (served p1)))
)
