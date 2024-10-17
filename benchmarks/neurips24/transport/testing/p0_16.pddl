(define (problem p0_16)
 (:domain transport)
 (:objects
   l6 l1 l4 l7 l8 l3 l2 l5 l10 l9 - location
   v1 v2 v3 v4 v5 - vehicle
   p1 p2 p3 p4 p5 p6 p7 p8 - package
 )
 (:init (= (capacity v1) 1) (= (capacity v2) 2) (= (capacity v3) 2) (= (capacity v4) 1) (= (capacity v5) 2) (at p1 l9) (at p2 l2) (at p3 l6) (at p4 l4) (at p5 l7) (at p6 l1) (at p7 l5) (at p8 l8) (at v1 l5) (at v2 l7) (at v3 l6) (at v4 l2) (at v5 l3) (road l4 l5) (road l9 l2) (road l2 l9) (road l9 l1) (road l1 l9) (road l9 l4) (road l9 l3) (road l9 l5) (road l4 l9) (road l3 l9) (road l5 l9) (road l1 l2) (road l3 l7) (road l7 l3) (road l2 l1) (road l2 l7) (road l7 l2) (road l2 l10) (road l10 l2) (road l10 l6) (road l10 l3) (road l6 l10) (road l3 l10) (road l4 l3) (road l3 l4) (road l3 l8) (road l8 l3) (road l3 l6) (road l5 l4) (road l6 l3))
 (:goal (and (at p1 l8) (at p2 l1) (at p3 l1) (at p4 l1) (at p5 l8) (at p6 l5) (at p7 l4) (at p8 l1)))
)
