(define (problem p0_02)
 (:domain transport)
 (:objects
   l1 l4 l3 l2 l5 - location
   v1 v2 v3 - vehicle
   p1 - package
 )
 (:init (= (capacity v1) 2) (= (capacity v2) 2) (= (capacity v3) 1) (at p1 l1) (at v1 l1) (at v2 l2) (at v3 l5) (road l5 l2) (road l4 l5) (road l2 l5) (road l4 l2) (road l2 l4) (road l3 l2) (road l2 l3) (road l3 l1) (road l1 l3) (road l5 l4))
 (:goal (and (at p1 l2)))
)
