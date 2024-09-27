(define (problem transport-problem)
 (:domain transport-domain)
 (:objects
   l6 l1 l4 l3 l2 l5 - location
   v1 v2 v3 - vehicle
   p1 p2 p3 - package
 )
 (:init (= (capacity v1) 1) (= (capacity v2) 2) (= (capacity v3) 2) (at p1 l4) (at p2 l4) (at p3 l4) (at v1 l2) (at v2 l4) (at v3 l3) (road l6 l5) (road l4 l5) (road l4 l2) (road l2 l4) (road l6 l1) (road l3 l2) (road l2 l3) (road l3 l1) (road l1 l6) (road l1 l3) (road l5 l6) (road l5 l4))
 (:goal (and (at p1 l3) (at p2 l5) (at p3 l1)))
)
