(define (problem ferry_12-problem)
 (:domain ferry_12-domain)
 (:objects
   car1 - car
   loc1 loc2 loc3 loc4 loc5 - location
 )
 (:init (at-ferry loc1) (at car1 loc1) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc4)))
)
