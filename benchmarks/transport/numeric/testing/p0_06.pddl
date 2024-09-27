(define (problem p0_06)
 (:domain transport)
 (:objects
   l6 l1 l4 l3 l2 l5 - location
   v1 v2 v3 - vehicle
   p1 p2 p3 - package
 )
 (:init (= (capacity v1) 2) (= (capacity v2) 1) (= (capacity v3) 1) (at p1 l4) (at p2 l2) (at p3 l5) (at v1 l1) (at v2 l3) (at v3 l3) (road l5 l2) (road l2 l5) (road l4 l2) (road l2 l4) (road l4 l3) (road l6 l2) (road l3 l4) (road l1 l5) (road l5 l1) (road l2 l6) (road l3 l1) (road l1 l3))
 (:goal (and (at p1 l5) (at p2 l4) (at p3 l6)))
)
