(define (problem transport-problem)
 (:domain transport-domain)
 (:objects
   l6 l1 l4 l7 l8 l12 l11 l3 l2 l5 l10 l9 - location
   v1 v2 v3 v4 v5 - vehicle
   p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 - package
 )
 (:init (= (capacity v1) 2) (= (capacity v2) 2) (= (capacity v3) 1) (= (capacity v4) 1) (= (capacity v5) 1) (at p1 l4) (at p2 l3) (at p3 l2) (at p4 l5) (at p5 l12) (at p6 l7) (at p7 l9) (at p8 l11) (at p9 l12) (at p10 l9) (at v1 l2) (at v2 l10) (at v3 l8) (at v4 l4) (at v5 l7) (road l6 l5) (road l4 l5) (road l3 l5) (road l12 l6) (road l9 l2) (road l2 l9) (road l9 l1) (road l1 l9) (road l9 l6) (road l9 l8) (road l9 l5) (road l6 l9) (road l8 l9) (road l5 l9) (road l10 l11) (road l11 l10) (road l7 l11) (road l11 l7) (road l1 l2) (road l2 l1) (road l3 l7) (road l7 l3) (road l2 l11) (road l11 l2) (road l1 l7) (road l7 l1) (road l5 l7) (road l7 l5) (road l3 l2) (road l2 l3) (road l3 l1) (road l1 l3) (road l4 l10) (road l10 l4) (road l2 l10) (road l10 l2) (road l10 l8) (road l8 l10) (road l4 l3) (road l3 l4) (road l9 l10) (road l10 l9) (road l12 l3) (road l3 l12) (road l9 l12) (road l12 l9) (road l7 l12) (road l12 l7) (road l5 l8) (road l8 l7) (road l2 l12) (road l12 l2) (road l8 l5) (road l7 l8) (road l2 l8) (road l8 l2) (road l3 l8) (road l8 l3) (road l10 l12) (road l12 l10) (road l6 l8) (road l8 l6) (road l12 l8) (road l8 l12) (road l12 l1) (road l4 l8) (road l8 l4) (road l6 l12) (road l1 l12) (road l4 l6) (road l3 l6) (road l5 l6) (road l6 l4) (road l5 l4) (road l6 l3) (road l5 l3))
 (:goal (and (at p1 l12) (at p2 l12) (at p3 l10) (at p4 l12) (at p5 l3) (at p6 l10) (at p7 l7) (at p8 l2) (at p9 l2) (at p10 l2)))
)
