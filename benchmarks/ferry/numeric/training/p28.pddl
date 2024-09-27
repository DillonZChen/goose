(define (problem ferry_28-problem)
 (:domain ferry_28-domain)
 (:objects
   car1 car2 car3 car4 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 - location
 )
 (:init (at-ferry loc5) (at car1 loc2) (at car2 loc2) (at car3 loc6) (at car4 loc6) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc3) (at car2 loc1) (at car3 loc2) (at car4 loc4)))
)
