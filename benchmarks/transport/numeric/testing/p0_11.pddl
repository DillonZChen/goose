(define (problem p0_11)
 (:domain transport)
 (:objects
   l6 l1 l4 l7 l8 l3 l2 l5 - location
   v1 v2 v3 v4 - vehicle
   p1 p2 p3 p4 p5 p6 - package
 )
 (:init (= (capacity v1) 2) (= (capacity v2) 2) (= (capacity v3) 1) (= (capacity v4) 1) (at p1 l6) (at p2 l5) (at p3 l2) (at p4 l5) (at p5 l5) (at p6 l1) (at v1 l5) (at v2 l8) (at v3 l7) (at v4 l8) (road l5 l2) (road l2 l5) (road l1 l2) (road l2 l1) (road l2 l7) (road l7 l2) (road l3 l7) (road l7 l3) (road l4 l2) (road l2 l4) (road l5 l8) (road l8 l5) (road l3 l8) (road l8 l3) (road l4 l8) (road l8 l4) (road l6 l1) (road l1 l6) (road l3 l6) (road l6 l3))
 (:goal (and (at p1 l2) (at p2 l7) (at p3 l6) (at p4 l7) (at p5 l7) (at p6 l2)))
)
