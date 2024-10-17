(define (problem transport-problem)
 (:domain transport-domain)
 (:objects
   l6 l1 l4 l7 l8 l3 l2 l5 l9 - location
   v1 v2 v3 v4 - vehicle
   p1 p2 p3 p4 p5 p6 - package
 )
 (:init (= (capacity v1) 2) (= (capacity v2) 2) (= (capacity v3) 2) (= (capacity v4) 1) (at p1 l9) (at p2 l8) (at p3 l6) (at p4 l2) (at p5 l6) (at p6 l2) (at v1 l5) (at v2 l5) (at v3 l8) (at v4 l6) (road l6 l5) (road l3 l7) (road l7 l3) (road l7 l4) (road l9 l2) (road l2 l9) (road l4 l7) (road l5 l6) (road l9 l1) (road l1 l9) (road l6 l2) (road l2 l6) (road l2 l3) (road l3 l2) (road l5 l8) (road l8 l5))
 (:goal (and (at p1 l1) (at p2 l5) (at p3 l8) (at p4 l5) (at p5 l1) (at p6 l9)))
)
