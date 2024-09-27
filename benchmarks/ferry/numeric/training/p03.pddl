(define (problem ferry_03-problem)
 (:domain ferry_03-domain)
 (:objects
   car1 - car
   loc1 loc2 - location
 )
 (:init (at-ferry loc1) (at car1 loc2) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc1)))
)
