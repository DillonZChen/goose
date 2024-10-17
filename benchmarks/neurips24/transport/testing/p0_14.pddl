(define (problem p0_14)
 (:domain transport)
 (:objects
   l6 l1 l4 l7 l8 l3 l2 l5 l9 - location
   v1 v2 v3 v4 - vehicle
   p1 p2 p3 p4 p5 p6 p7 - package
 )
 (:init (= (capacity v1) 1) (= (capacity v2) 1) (= (capacity v3) 2) (= (capacity v4) 2) (at p1 l3) (at p2 l5) (at p3 l3) (at p4 l5) (at p5 l6) (at p6 l7) (at p7 l9) (at v1 l9) (at v2 l5) (at v3 l5) (at v4 l7) (road l5 l2) (road l3 l7) (road l7 l3) (road l3 l8) (road l4 l5) (road l2 l5) (road l8 l3) (road l6 l3) (road l4 l3) (road l3 l4) (road l9 l4) (road l4 l9) (road l8 l1) (road l3 l6) (road l5 l4) (road l1 l8))
 (:goal (and (at p1 l7) (at p2 l9) (at p3 l1) (at p4 l3) (at p5 l5) (at p6 l5) (at p7 l2)))
)
