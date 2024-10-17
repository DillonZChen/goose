(define (problem p0_04)
 (:domain miconic)
 (:objects
   p1 p2 - passenger
   f1 f2 f3 f4 f5 - floor
 )
 (:init (lift-at f5) (origin p1 f2) (destin p1 f1) (origin p2 f4) (destin p2 f5) (above f1 f2) (above f1 f3) (above f1 f4) (above f1 f5) (above f2 f3) (above f2 f4) (above f2 f5) (above f3 f4) (above f3 f5) (above f4 f5) (= (lift-capacity) 4) (= (weight p1) 1) (= (weight p2) 1))
 (:goal (and (served p1) (served p2)))
)
