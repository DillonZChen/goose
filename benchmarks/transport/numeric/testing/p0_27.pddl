(define (problem p0_27)
 (:domain transport)
 (:objects
   l6 l1 l4 l7 l14 l8 l11 l12 l13 l3 l2 l5 l10 l9 - location
   v1 v2 v3 v4 v5 v6 - vehicle
   p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 - package
 )
 (:init (= (capacity v1) 1) (= (capacity v2) 2) (= (capacity v3) 2) (= (capacity v4) 1) (= (capacity v5) 1) (= (capacity v6) 1) (at p1 l1) (at p2 l7) (at p3 l7) (at p4 l3) (at p5 l13) (at p6 l14) (at p7 l3) (at p8 l2) (at p9 l14) (at p10 l3) (at p11 l11) (at p12 l7) (at p13 l1) (at p14 l3) (at v1 l8) (at v2 l7) (at v3 l2) (at v4 l11) (at v5 l8) (at v6 l10) (road l3 l5) (road l14 l13) (road l13 l14) (road l8 l14) (road l14 l8) (road l9 l13) (road l13 l9) (road l10 l13) (road l13 l10) (road l3 l7) (road l7 l3) (road l2 l13) (road l13 l2) (road l6 l11) (road l3 l11) (road l11 l3) (road l6 l13) (road l13 l6) (road l11 l4) (road l4 l11) (road l13 l4) (road l4 l13) (road l11 l6) (road l5 l10) (road l10 l5) (road l2 l10) (road l10 l2) (road l10 l6) (road l10 l1) (road l10 l3) (road l6 l10) (road l1 l10) (road l3 l10) (road l8 l7) (road l8 l1) (road l1 l8) (road l7 l8) (road l6 l12) (road l12 l6) (road l5 l3))
 (:goal (and (at p1 l3) (at p2 l10) (at p3 l13) (at p4 l1) (at p5 l12) (at p6 l3) (at p7 l9) (at p8 l8) (at p9 l5) (at p10 l8) (at p11 l10) (at p12 l9) (at p13 l2) (at p14 l7)))
)
