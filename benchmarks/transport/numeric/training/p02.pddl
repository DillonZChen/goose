(define (problem transport-problem)
 (:domain transport-domain)
 (:objects
   l1 l2 - location
   v1 - vehicle
   p1 - package
 )
 (:init (= (capacity v1) 1) (at p1 l1) (at v1 l2) (road l1 l2) (road l2 l1))
 (:goal (and (at p1 l2)))
)
