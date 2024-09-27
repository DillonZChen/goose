(define (problem transport-problem)
 (:domain transport-domain)
 (:objects
   l1 l3 l2 - location
   v1 - vehicle
   p1 - package
 )
 (:init (= (capacity v1) 1) (at p1 l3) (at v1 l1) (road l1 l2) (road l2 l1) (road l3 l2) (road l2 l3))
 (:goal (and (at p1 l1)))
)
