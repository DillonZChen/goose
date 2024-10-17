(define (problem ferry_35-problem)
 (:domain ferry_35-domain)
 (:objects
   car1 car2 car3 car4 car5 car6 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 - location
 )
 (:init (at-ferry loc4) (at car1 loc3) (at car2 loc3) (at car3 loc5) (at car4 loc7) (at car5 loc2) (at car6 loc4) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc5) (at car2 loc6) (at car3 loc4) (at car4 loc5) (at car5 loc4) (at car6 loc7)))
)
