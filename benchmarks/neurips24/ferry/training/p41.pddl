(define (problem ferry_41-problem)
 (:domain ferry_41-domain)
 (:objects
   car1 car2 car3 car4 car5 car6 car7 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 - location
 )
 (:init (at-ferry loc6) (at car1 loc1) (at car2 loc5) (at car3 loc3) (at car4 loc4) (at car5 loc5) (at car6 loc2) (at car7 loc4) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc6) (at car2 loc1) (at car3 loc6) (at car4 loc2) (at car5 loc6) (at car6 loc1) (at car7 loc8)))
)
