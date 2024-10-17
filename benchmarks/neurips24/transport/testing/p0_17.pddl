(define (problem p0_17)
 (:domain transport)
 (:objects
   l6 l1 l4 l7 l8 l3 l2 l5 l10 l9 - location
   v1 v2 v3 v4 v5 - vehicle
   p1 p2 p3 p4 p5 p6 p7 p8 p9 - package
 )
 (:init (= (capacity v1) 2) (= (capacity v2) 2) (= (capacity v3) 2) (= (capacity v4) 1) (= (capacity v5) 2) (at p1 l5) (at p2 l1) (at p3 l7) (at p4 l6) (at p5 l6) (at p6 l6) (at p7 l7) (at p8 l7) (at p9 l5) (at v1 l2) (at v2 l9) (at v3 l5) (at v4 l8) (at v5 l2) (road l5 l2) (road l2 l5) (road l9 l2) (road l2 l9) (road l9 l4) (road l9 l8) (road l4 l9) (road l8 l9) (road l2 l7) (road l7 l2) (road l1 l7) (road l7 l1) (road l7 l6) (road l6 l7) (road l5 l7) (road l7 l5) (road l3 l1) (road l1 l3) (road l7 l10) (road l10 l7) (road l10 l1) (road l10 l8) (road l1 l10) (road l8 l10) (road l4 l2) (road l2 l4) (road l4 l3) (road l3 l4) (road l8 l7) (road l7 l8) (road l6 l1) (road l1 l6))
 (:goal (and (at p1 l6) (at p2 l10) (at p3 l6) (at p4 l4) (at p5 l1) (at p6 l4) (at p7 l5) (at p8 l8) (at p9 l9)))
)
