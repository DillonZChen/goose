(define (problem transport-problem)
 (:domain transport-domain)
 (:objects
   l6 l1 l4 l7 l8 l3 l2 l5 l10 l9 - location
   v1 v2 v3 v4 v5 - vehicle
   p1 p2 p3 p4 p5 p6 p7 p8 - package
 )
 (:init (= (capacity v1) 2) (= (capacity v2) 1) (= (capacity v3) 2) (= (capacity v4) 1) (= (capacity v5) 1) (at p1 l9) (at p2 l1) (at p3 l6) (at p4 l8) (at p5 l3) (at p6 l4) (at p7 l2) (at p8 l3) (at v1 l10) (at v2 l3) (at v3 l8) (at v4 l7) (at v5 l6) (road l3 l5) (road l3 l7) (road l7 l3) (road l2 l10) (road l10 l2) (road l10 l1) (road l10 l8) (road l1 l10) (road l8 l10) (road l9 l10) (road l10 l9) (road l5 l8) (road l8 l1) (road l1 l8) (road l8 l5) (road l4 l8) (road l8 l4) (road l6 l8) (road l8 l6) (road l5 l3))
 (:goal (and (at p1 l3) (at p2 l6) (at p3 l5) (at p4 l7) (at p5 l9) (at p6 l10) (at p7 l6) (at p8 l7)))
)
