(define (problem transport-problem)
 (:domain transport-domain)
 (:objects
   l6 l1 l4 l7 l8 l3 l2 l5 l10 l9 - location
   v1 v2 v3 v4 v5 - vehicle
   p1 p2 p3 p4 p5 p6 p7 p8 - package
 )
 (:init (= (capacity v1) 1) (= (capacity v2) 1) (= (capacity v3) 1) (= (capacity v4) 2) (= (capacity v5) 2) (at p1 l9) (at p2 l6) (at p3 l2) (at p4 l6) (at p5 l1) (at p6 l5) (at p7 l7) (at p8 l2) (at v1 l8) (at v2 l9) (at v3 l10) (at v4 l2) (at v5 l4) (road l6 l5) (road l9 l7) (road l7 l9) (road l1 l2) (road l2 l1) (road l5 l7) (road l7 l5) (road l3 l2) (road l2 l3) (road l7 l10) (road l10 l7) (road l10 l8) (road l8 l10) (road l8 l1) (road l1 l8) (road l2 l8) (road l8 l2) (road l6 l8) (road l8 l6) (road l4 l6) (road l5 l6) (road l6 l4))
 (:goal (and (at p1 l6) (at p2 l4) (at p3 l1) (at p4 l5) (at p5 l5) (at p6 l6) (at p7 l4) (at p8 l4)))
)
