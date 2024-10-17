(define (problem ferry_48-problem)
 (:domain ferry_48-domain)
 (:objects
   car1 car2 car3 car4 car5 car6 car7 car8 car9 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 - location
 )
 (:init (at-ferry loc4) (at car1 loc2) (at car2 loc5) (at car3 loc5) (at car4 loc7) (at car5 loc1) (at car6 loc3) (at car7 loc7) (at car8 loc4) (at car9 loc8) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc8) (at car2 loc1) (at car3 loc4) (at car4 loc3) (at car5 loc7) (at car6 loc8) (at car7 loc6) (at car8 loc8) (at car9 loc3)))
)
