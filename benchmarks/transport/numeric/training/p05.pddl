(define (problem transport-problem)
 (:domain transport-domain)
 (:objects
   l1 l2 - location
   v1 - vehicle
   p1 p2 - package
 )
 (:init (= (capacity v1) 2) (at p1 l1) (at p2 l1) (at v1 l1) (road l1 l2) (road l2 l1))
 (:goal (and (at p1 l2) (at p2 l2)))
)
