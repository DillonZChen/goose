(define (problem p0_25)
 (:domain transport)
 (:objects
   l6 l1 l4 l7 l8 l11 l12 l13 l3 l2 l5 l10 l9 - location
   v1 v2 v3 v4 v5 v6 - vehicle
   p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 - package
 )
 (:init (= (capacity v1) 1) (= (capacity v2) 1) (= (capacity v3) 2) (= (capacity v4) 2) (= (capacity v5) 1) (= (capacity v6) 2) (at p1 l11) (at p2 l5) (at p3 l8) (at p4 l13) (at p5 l7) (at p6 l8) (at p7 l6) (at p8 l1) (at p9 l8) (at p10 l3) (at p11 l1) (at p12 l3) (at p13 l4) (at v1 l10) (at v2 l13) (at v3 l12) (at v4 l10) (at v5 l1) (at v6 l10) (road l13 l8) (road l8 l13) (road l9 l13) (road l13 l9) (road l5 l13) (road l13 l5) (road l1 l2) (road l2 l1) (road l11 l13) (road l13 l11) (road l3 l13) (road l13 l3) (road l6 l13) (road l13 l6) (road l3 l2) (road l2 l3) (road l3 l1) (road l1 l3) (road l1 l4) (road l4 l1) (road l1 l5) (road l6 l1) (road l5 l1) (road l1 l6) (road l5 l4) (road l5 l3) (road l4 l5) (road l3 l5) (road l3 l7) (road l7 l3) (road l1 l7) (road l7 l1) (road l7 l4) (road l4 l7) (road l8 l7) (road l8 l1) (road l1 l8) (road l7 l8) (road l2 l8) (road l8 l2) (road l6 l8) (road l8 l6) (road l9 l2) (road l2 l9) (road l9 l1) (road l1 l9) (road l9 l7) (road l9 l8) (road l7 l9) (road l8 l9) (road l7 l10) (road l10 l7) (road l5 l10) (road l10 l5) (road l2 l10) (road l10 l2) (road l10 l6) (road l6 l10) (road l11 l1) (road l1 l11) (road l5 l11) (road l11 l5) (road l10 l11) (road l11 l10) (road l8 l11) (road l11 l8) (road l6 l11) (road l11 l6) (road l3 l11) (road l11 l3) (road l11 l4) (road l4 l11) (road l12 l3) (road l3 l12) (road l9 l12) (road l12 l9) (road l7 l12) (road l12 l7) (road l10 l12) (road l12 l10) (road l8 l12) (road l12 l8) (road l12 l4) (road l4 l12) (road l11 l12) (road l12 l11))
 (:goal (and (at p1 l13) (at p2 l9) (at p3 l12) (at p4 l11) (at p5 l2) (at p6 l2) (at p7 l2) (at p8 l8) (at p9 l5) (at p10 l9) (at p11 l11) (at p12 l7) (at p13 l1)))
)
