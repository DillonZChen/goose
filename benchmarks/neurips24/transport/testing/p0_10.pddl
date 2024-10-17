(define (problem p0_10)
 (:domain transport)
 (:objects
   l6 l1 l4 l7 l8 l3 l2 l5 - location
   v1 v2 v3 v4 - vehicle
   p1 p2 p3 p4 p5 - package
 )
 (:init (= (capacity v1) 2) (= (capacity v2) 2) (= (capacity v3) 2) (= (capacity v4) 1) (at p1 l2) (at p2 l6) (at p3 l8) (at p4 l1) (at p5 l8) (at v1 l8) (at v2 l7) (at v3 l3) (at v4 l8) (road l4 l5) (road l3 l5) (road l7 l4) (road l4 l7) (road l5 l7) (road l7 l5) (road l3 l2) (road l2 l3) (road l3 l1) (road l1 l3) (road l4 l2) (road l2 l4) (road l5 l8) (road l8 l7) (road l8 l1) (road l1 l8) (road l8 l5) (road l7 l8) (road l2 l8) (road l8 l2) (road l6 l8) (road l8 l6) (road l1 l5) (road l5 l1) (road l4 l6) (road l3 l6) (road l6 l4) (road l5 l4) (road l6 l3) (road l5 l3))
 (:goal (and (at p1 l4) (at p2 l3) (at p3 l4) (at p4 l5) (at p5 l5)))
)
