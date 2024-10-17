(define (problem p0_26)
 (:domain transport)
 (:objects
   l6 l1 l4 l7 l14 l8 l12 l11 l3 l13 l2 l5 l10 l9 - location
   v1 v2 v3 v4 v5 v6 - vehicle
   p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 - package
 )
 (:init (= (capacity v1) 2) (= (capacity v2) 2) (= (capacity v3) 1) (= (capacity v4) 2) (= (capacity v5) 1) (= (capacity v6) 2) (at p1 l5) (at p2 l12) (at p3 l4) (at p4 l1) (at p5 l6) (at p6 l1) (at p7 l4) (at p8 l5) (at p9 l7) (at p10 l7) (at p11 l3) (at p12 l9) (at p13 l6) (at v1 l5) (at v2 l9) (at v3 l1) (at v4 l12) (at v5 l14) (at v6 l1) (road l6 l5) (road l9 l1) (road l1 l9) (road l14 l1) (road l9 l13) (road l5 l11) (road l9 l7) (road l8 l14) (road l14 l8) (road l13 l9) (road l11 l5) (road l10 l14) (road l7 l9) (road l14 l10) (road l9 l11) (road l4 l14) (road l14 l4) (road l3 l14) (road l14 l3) (road l2 l14) (road l14 l2) (road l2 l7) (road l11 l9) (road l6 l11) (road l2 l13) (road l9 l14) (road l13 l2) (road l11 l6) (road l14 l9) (road l7 l4) (road l7 l2) (road l4 l7) (road l4 l11) (road l11 l4) (road l1 l7) (road l7 l1) (road l13 l3) (road l7 l13) (road l13 l7) (road l4 l13) (road l13 l4) (road l4 l10) (road l10 l4) (road l5 l10) (road l10 l5) (road l2 l10) (road l10 l2) (road l10 l6) (road l6 l10) (road l1 l4) (road l4 l1) (road l3 l13) (road l12 l3) (road l3 l12) (road l6 l13) (road l13 l6) (road l8 l1) (road l1 l8) (road l2 l8) (road l8 l2) (road l3 l8) (road l8 l3) (road l4 l8) (road l8 l4) (road l6 l8) (road l8 l6) (road l12 l4) (road l4 l12) (road l1 l14) (road l5 l6))
 (:goal (and (at p1 l10) (at p2 l7) (at p3 l10) (at p4 l13) (at p5 l14) (at p6 l3) (at p7 l1) (at p8 l3) (at p9 l14) (at p10 l14) (at p11 l4) (at p12 l10) (at p13 l5)))
)
