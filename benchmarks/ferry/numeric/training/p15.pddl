(define (problem ferry_15-problem)
 (:domain ferry_15-domain)
 (:objects
   car1 - car
   loc1 loc2 loc3 loc4 loc5 - location
 )
 (:init (at-ferry loc3) (at car1 loc4) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc5)))
)
