(define (problem transport-problem)
 (:domain transport-domain)
 (:objects
   l6 l1 l4 l7 l14 l8 l12 l11 l3 l2 l13 l15 l5 l10 l9 l16 - location
   v1 v2 v3 v4 v5 v6 v7 - vehicle
   p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 p15 p16 - package
 )
 (:init (= (capacity v1) 3) (= (capacity v2) 2) (= (capacity v3) 3) (= (capacity v4) 3) (= (capacity v5) 3) (= (capacity v6) 2) (= (capacity v7) 2) (at p1 l10) (at p2 l4) (at p3 l15) (at p4 l10) (at p5 l1) (at p6 l5) (at p7 l12) (at p8 l9) (at p9 l12) (at p10 l12) (at p11 l10) (at p12 l9) (at p13 l9) (at p14 l9) (at p15 l9) (at p16 l2) (at v1 l7) (at v2 l10) (at v3 l1) (at v4 l9) (at v5 l3) (at v6 l10) (at v7 l9) (road l9 l1) (road l1 l9) (road l9 l4) (road l9 l8) (road l4 l9) (road l9 l11) (road l9 l13) (road l8 l9) (road l11 l9) (road l13 l9) (road l2 l14) (road l14 l2) (road l1 l2) (road l3 l7) (road l7 l3) (road l12 l14) (road l2 l1) (road l9 l14) (road l14 l12) (road l14 l9) (road l11 l13) (road l13 l11) (road l16 l1) (road l16 l3) (road l1 l16) (road l10 l8) (road l1 l4) (road l4 l1) (road l8 l10) (road l3 l16) (road l15 l13) (road l13 l15) (road l14 l15) (road l15 l14) (road l6 l15) (road l15 l6) (road l8 l7) (road l5 l15) (road l15 l5) (road l7 l8) (road l6 l12) (road l12 l6) (road l15 l7) (road l15 l8) (road l7 l15) (road l8 l15) (road l10 l15) (road l6 l1) (road l15 l10) (road l1 l6))
 (:goal (and (at p1 l6) (at p2 l11) (at p3 l8) (at p4 l3) (at p5 l5) (at p6 l16) (at p7 l1) (at p8 l13) (at p9 l7) (at p10 l11) (at p11 l11) (at p12 l10) (at p13 l8) (at p14 l15) (at p15 l5) (at p16 l8)))
)
