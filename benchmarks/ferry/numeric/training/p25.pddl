(define (problem ferry_25-problem)
 (:domain ferry_25-domain)
 (:objects
   car1 car2 car3 - car
   loc1 loc2 loc3 loc4 loc5 loc6 - location
 )
 (:init (at-ferry loc1) (at car1 loc2) (at car2 loc2) (at car3 loc6) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc4) (at car2 loc1) (at car3 loc2)))
)
