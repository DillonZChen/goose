(define (problem transport-problem)
 (:domain transport-domain)
 (:objects
   l6 l1 l4 l7 l8 l11 l3 l2 l5 l10 l9 - location
   v1 v2 v3 v4 v5 - vehicle
   p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 - package
 )
 (:init (= (capacity v1) 2) (= (capacity v2) 2) (= (capacity v3) 2) (= (capacity v4) 1) (= (capacity v5) 1) (at p1 l6) (at p2 l1) (at p3 l1) (at p4 l3) (at p5 l7) (at p6 l11) (at p7 l6) (at p8 l2) (at p9 l5) (at p10 l6) (at v1 l6) (at v2 l5) (at v3 l3) (at v4 l9) (at v5 l1) (road l9 l2) (road l2 l9) (road l9 l6) (road l6 l9) (road l11 l1) (road l1 l11) (road l10 l11) (road l11 l10) (road l7 l11) (road l11 l7) (road l1 l2) (road l2 l1) (road l1 l7) (road l7 l1) (road l11 l4) (road l4 l11) (road l4 l10) (road l10 l4) (road l5 l10) (road l10 l5) (road l10 l6) (road l6 l10) (road l4 l2) (road l2 l4) (road l3 l8) (road l8 l3) (road l6 l2) (road l1 l5) (road l6 l1) (road l5 l1) (road l2 l6) (road l1 l6) (road l4 l6) (road l3 l6) (road l6 l4) (road l6 l3))
 (:goal (and (at p1 l11) (at p2 l3) (at p3 l11) (at p4 l1) (at p5 l6) (at p6 l3) (at p7 l11) (at p8 l9) (at p9 l4) (at p10 l3)))
)
