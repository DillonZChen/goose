(define (problem ferry_32-problem)
 (:domain ferry_32-domain)
 (:objects
   car1 car2 car3 car4 car5 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 - location
 )
 (:init (at-ferry loc5) (at car1 loc2) (at car2 loc1) (at car3 loc7) (at car4 loc2) (at car5 loc4) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc4) (at car2 loc7) (at car3 loc3) (at car4 loc3) (at car5 loc6)))
)
