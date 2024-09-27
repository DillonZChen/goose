(define (problem p0_01)
 (:domain transport)
 (:objects
   l1 l4 l3 l2 l5 - location
   v1 v2 v3 - vehicle
   p1 - package
 )
 (:init (= (capacity v1) 1) (= (capacity v2) 1) (= (capacity v3) 2) (at p1 l2) (at v1 l2) (at v2 l5) (at v3 l1) (road l4 l5) (road l1 l4) (road l4 l1) (road l4 l3) (road l3 l4) (road l3 l2) (road l2 l3) (road l3 l1) (road l1 l3) (road l5 l4))
 (:goal (and (at p1 l3)))
)
