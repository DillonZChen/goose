(define (problem ferry_26-problem)
 (:domain ferry_26-domain)
 (:objects
   car1 car2 car3 car4 - car
   loc1 loc2 loc3 loc4 loc5 loc6 - location
 )
 (:init (at-ferry loc5) (at car1 loc1) (at car2 loc4) (at car3 loc6) (at car4 loc5) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc4) (at car2 loc6) (at car3 loc4) (at car4 loc2)))
)
