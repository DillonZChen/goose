(define (problem p0_08)
 (:domain transport)
 (:objects
   l6 l1 l4 l7 l3 l2 l5 - location
   v1 v2 v3 - vehicle
   p1 p2 p3 p4 - package
 )
 (:init (= (capacity v1) 1) (= (capacity v2) 1) (= (capacity v3) 2) (at p1 l1) (at p2 l4) (at p3 l4) (at p4 l6) (at v1 l5) (at v2 l5) (at v3 l2) (road l4 l5) (road l1 l2) (road l2 l1) (road l2 l7) (road l7 l2) (road l7 l4) (road l4 l7) (road l5 l7) (road l7 l5) (road l3 l1) (road l1 l3) (road l4 l2) (road l2 l4) (road l4 l3) (road l3 l4) (road l6 l1) (road l1 l6) (road l3 l6) (road l5 l4) (road l6 l3))
 (:goal (and (at p1 l3) (at p2 l5) (at p3 l1) (at p4 l1)))
)
