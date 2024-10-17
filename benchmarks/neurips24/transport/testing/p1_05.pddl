(define (problem p1_05)
 (:domain transport)
 (:objects
   l22 l8 l17 l5 l20 l1 l7 l14 l2 l10 l16 l4 l3 l9 l6 l19 l12 l11 l13 l15 l21 l18 - location
   v1 v2 v3 v4 v5 v6 v7 v8 v9 v10 v11 - vehicle
   p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 - package
 )
 (:init (= (capacity v1) 3) (= (capacity v2) 3) (= (capacity v3) 1) (= (capacity v4) 4) (= (capacity v5) 2) (= (capacity v6) 4) (= (capacity v7) 4) (= (capacity v8) 4) (= (capacity v9) 3) (= (capacity v10) 2) (= (capacity v11) 1) (at p1 l15) (at p2 l22) (at p3 l10) (at p4 l13) (at p5 l20) (at p6 l4) (at p7 l12) (at p8 l7) (at p9 l3) (at p10 l14) (at v1 l19) (at v2 l12) (at v3 l16) (at v4 l1) (at v5 l16) (at v6 l6) (at v7 l18) (at v8 l1) (at v9 l22) (at v10 l9) (at v11 l17) (road l15 l19) (road l3 l5) (road l17 l22) (road l22 l17) (road l19 l15) (road l14 l18) (road l9 l4) (road l8 l14) (road l14 l8) (road l4 l9) (road l9 l13) (road l5 l11) (road l11 l5) (road l20 l21) (road l21 l20) (road l13 l9) (road l21 l17) (road l11 l13) (road l13 l11) (road l12 l14) (road l2 l7) (road l7 l2) (road l14 l12) (road l7 l4) (road l4 l7) (road l4 l11) (road l11 l4) (road l17 l20) (road l17 l21) (road l20 l17) (road l2 l16) (road l20 l5) (road l5 l20) (road l16 l2) (road l16 l1) (road l11 l20) (road l20 l11) (road l16 l8) (road l1 l16) (road l8 l16) (road l7 l20) (road l10 l20) (road l15 l17) (road l4 l20) (road l17 l15) (road l9 l12) (road l12 l9) (road l19 l1) (road l19 l14) (road l18 l14) (road l20 l7) (road l8 l12) (road l1 l12) (road l12 l1) (road l6 l12) (road l20 l10) (road l12 l6) (road l12 l8) (road l6 l3) (road l16 l22) (road l22 l16) (road l20 l4) (road l4 l22) (road l22 l4) (road l10 l15) (road l1 l5) (road l5 l1) (road l15 l10) (road l1 l6) (road l14 l19) (road l6 l1) (road l1 l19) (road l3 l6) (road l1 l22) (road l22 l1) (road l5 l3))
 (:goal (and (at p1 l14) (at p2 l4) (at p3 l5) (at p4 l21) (at p5 l21) (at p6 l13) (at p7 l10) (at p8 l3) (at p9 l13) (at p10 l20)))
)
