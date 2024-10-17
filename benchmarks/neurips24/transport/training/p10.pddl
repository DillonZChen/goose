(define (problem transport-problem)
 (:domain transport-domain)
 (:objects
   l1 l4 l3 l2 - location
   v1 v2 - vehicle
   p1 p2 p3 p4 - package
 )
 (:init (= (capacity v1) 2) (= (capacity v2) 2) (at p1 l3) (at p2 l3) (at p3 l2) (at p4 l1) (at v1 l4) (at v2 l2) (road l1 l4) (road l4 l1) (road l4 l2) (road l2 l4) (road l3 l2) (road l2 l3))
 (:goal (and (at p1 l4) (at p2 l2) (at p3 l3) (at p4 l4)))
)
