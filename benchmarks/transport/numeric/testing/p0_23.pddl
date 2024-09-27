(define (problem p0_23)
 (:domain transport)
 (:objects
   l6 l1 l4 l7 l8 l11 l12 l3 l2 l13 l5 l10 l9 - location
   v1 v2 v3 v4 v5 - vehicle
   p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 - package
 )
 (:init (= (capacity v1) 2) (= (capacity v2) 1) (= (capacity v3) 2) (= (capacity v4) 2) (= (capacity v5) 2) (at p1 l6) (at p2 l2) (at p3 l5) (at p4 l6) (at p5 l5) (at p6 l7) (at p7 l11) (at p8 l9) (at p9 l8) (at p10 l6) (at p11 l9) (at p12 l8) (at v1 l12) (at v2 l2) (at v3 l8) (at v4 l4) (at v5 l1) (road l6 l5) (road l3 l5) (road l9 l2) (road l2 l9) (road l9 l6) (road l9 l7) (road l9 l3) (road l9 l5) (road l6 l9) (road l11 l1) (road l1 l11) (road l7 l9) (road l8 l13) (road l5 l9) (road l13 l8) (road l3 l9) (road l1 l13) (road l2 l7) (road l11 l13) (road l6 l11) (road l11 l6) (road l7 l2) (road l2 l11) (road l11 l2) (road l13 l11) (road l7 l4) (road l2 l13) (road l13 l4) (road l4 l7) (road l4 l13) (road l13 l2) (road l13 l1) (road l4 l10) (road l10 l4) (road l10 l6) (road l6 l10) (road l4 l3) (road l3 l4) (road l12 l3) (road l3 l12) (road l7 l12) (road l12 l7) (road l8 l7) (road l8 l1) (road l1 l8) (road l7 l8) (road l1 l5) (road l5 l1) (road l4 l6) (road l5 l6) (road l6 l4) (road l5 l3))
 (:goal (and (at p1 l5) (at p2 l12) (at p3 l6) (at p4 l2) (at p5 l8) (at p6 l4) (at p7 l4) (at p8 l4) (at p9 l3) (at p10 l9) (at p11 l11) (at p12 l3)))
)
