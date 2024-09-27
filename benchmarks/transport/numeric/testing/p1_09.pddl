(define (problem p1_09)
 (:domain transport)
 (:objects
   l22 l8 l17 l5 l23 l20 l25 l1 l14 l7 l2 l24 l10 l16 l4 l3 l9 l6 l19 l11 l12 l13 l15 l21 l18 - location
   v1 v2 v3 v4 v5 v6 v7 v8 v9 v10 v11 v12 - vehicle
   p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 p15 - package
 )
 (:init (= (capacity v1) 1) (= (capacity v2) 3) (= (capacity v3) 4) (= (capacity v4) 4) (= (capacity v5) 3) (= (capacity v6) 4) (= (capacity v7) 3) (= (capacity v8) 4) (= (capacity v9) 1) (= (capacity v10) 1) (= (capacity v11) 4) (= (capacity v12) 4) (at p1 l14) (at p2 l17) (at p3 l11) (at p4 l21) (at p5 l21) (at p6 l24) (at p7 l8) (at p8 l23) (at p9 l8) (at p10 l10) (at p11 l2) (at p12 l8) (at p13 l20) (at p14 l13) (at p15 l19) (at v1 l4) (at v2 l3) (at v3 l9) (at v4 l14) (at v5 l10) (at v6 l19) (at v7 l15) (at v8 l18) (at v9 l19) (at v10 l9) (at v11 l11) (at v12 l19) (road l20 l15) (road l9 l2) (road l2 l9) (road l17 l18) (road l9 l6) (road l9 l4) (road l9 l5) (road l6 l9) (road l4 l9) (road l5 l9) (road l2 l13) (road l13 l2) (road l21 l12) (road l24 l16) (road l18 l19) (road l19 l18) (road l16 l24) (road l18 l24) (road l24 l18) (road l7 l24) (road l24 l7) (road l20 l12) (road l12 l20) (road l12 l21) (road l25 l23) (road l23 l25) (road l18 l21) (road l21 l18) (road l13 l18) (road l18 l13) (road l5 l10) (road l10 l5) (road l20 l8) (road l16 l10) (road l14 l24) (road l24 l14) (road l10 l1) (road l18 l17) (road l19 l8) (road l8 l20) (road l1 l10) (road l3 l17) (road l17 l3) (road l10 l9) (road l9 l10) (road l10 l16) (road l3 l20) (road l15 l20) (road l11 l15) (road l15 l11) (road l20 l22) (road l22 l20) (road l20 l3) (road l3 l23) (road l23 l3) (road l10 l15) (road l8 l19) (road l15 l10))
 (:goal (and (at p1 l8) (at p2 l16) (at p3 l22) (at p4 l20) (at p5 l18) (at p6 l12) (at p7 l18) (at p8 l3) (at p9 l14) (at p10 l16) (at p11 l18) (at p12 l4) (at p13 l17) (at p14 l12) (at p15 l17)))
)
