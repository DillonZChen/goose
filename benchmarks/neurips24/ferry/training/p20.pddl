(define (problem ferry_20-problem)
 (:domain ferry_20-domain)
 (:objects
   car1 car2 - car
   loc1 loc2 loc3 loc4 loc5 loc6 - location
 )
 (:init (at-ferry loc4) (at car1 loc3) (at car2 loc3) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc2) (at car2 loc5)))
)
