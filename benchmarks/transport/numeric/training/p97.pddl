(define (problem transport-problem)
 (:domain transport-domain)
 (:objects
   l6 l1 l4 l7 l14 l8 l11 l12 l3 l2 l13 l15 l5 l10 l9 l16 - location
   v1 v2 v3 v4 v5 v6 v7 - vehicle
   p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 p15 p16 p17 - package
 )
 (:init (= (capacity v1) 1) (= (capacity v2) 2) (= (capacity v3) 2) (= (capacity v4) 3) (= (capacity v5) 2) (= (capacity v6) 1) (= (capacity v7) 3) (at p1 l9) (at p2 l8) (at p3 l6) (at p4 l5) (at p5 l15) (at p6 l6) (at p7 l11) (at p8 l7) (at p9 l3) (at p10 l16) (at p11 l3) (at p12 l5) (at p13 l8) (at p14 l5) (at p15 l11) (at p16 l1) (at p17 l12) (at v1 l8) (at v2 l2) (at v3 l5) (at v4 l9) (at v5 l14) (at v6 l5) (at v7 l10) (road l15 l3) (road l9 l2) (road l2 l9) (road l9 l1) (road l1 l9) (road l14 l13) (road l13 l14) (road l8 l14) (road l14 l8) (road l9 l11) (road l11 l9) (road l10 l13) (road l13 l10) (road l11 l13) (road l13 l11) (road l2 l13) (road l1 l7) (road l7 l1) (road l2 l11) (road l6 l13) (road l13 l6) (road l11 l2) (road l13 l2) (road l9 l14) (road l5 l7) (road l7 l5) (road l14 l9) (road l2 l16) (road l16 l2) (road l3 l1) (road l1 l3) (road l4 l10) (road l10 l4) (road l16 l3) (road l10 l6) (road l6 l10) (road l1 l4) (road l4 l1) (road l3 l16) (road l4 l3) (road l3 l4) (road l12 l3) (road l3 l12) (road l5 l8) (road l2 l12) (road l8 l1) (road l1 l8) (road l5 l15) (road l15 l5) (road l8 l5) (road l12 l2) (road l3 l8) (road l8 l3) (road l2 l8) (road l8 l2) (road l6 l8) (road l8 l6) (road l11 l15) (road l15 l12) (road l15 l11) (road l11 l12) (road l12 l11) (road l12 l15) (road l1 l5) (road l5 l1) (road l3 l6) (road l3 l15) (road l6 l3))
 (:goal (and (at p1 l11) (at p2 l11) (at p3 l16) (at p4 l16) (at p5 l3) (at p6 l11) (at p7 l8) (at p8 l12) (at p9 l7) (at p10 l8) (at p11 l4) (at p12 l3) (at p13 l4) (at p14 l6) (at p15 l15) (at p16 l11) (at p17 l6)))
)
