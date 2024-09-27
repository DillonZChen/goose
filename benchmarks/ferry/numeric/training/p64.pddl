(define (problem ferry_64-problem)
 (:domain ferry_64-domain)
 (:objects
   car1 car2 car3 car4 car5 car6 car7 car8 car9 car10 car11 car12 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 loc10 loc11 - location
 )
 (:init (at-ferry loc9) (at car1 loc3) (at car2 loc2) (at car3 loc5) (at car4 loc5) (at car5 loc7) (at car6 loc1) (at car7 loc11) (at car8 loc8) (at car9 loc6) (at car10 loc5) (at car11 loc7) (at car12 loc10) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc6) (at car2 loc10) (at car3 loc1) (at car4 loc8) (at car5 loc1) (at car6 loc8) (at car7 loc6) (at car8 loc9) (at car9 loc7) (at car10 loc4) (at car11 loc1) (at car12 loc7)))
)
