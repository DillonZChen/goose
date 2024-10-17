(define (problem ferry_58-problem)
 (:domain ferry_58-domain)
 (:objects
   car1 car2 car3 car4 car5 car6 car7 car8 car9 car10 car11 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 loc10 - location
 )
 (:init (at-ferry loc7) (at car1 loc4) (at car2 loc6) (at car3 loc3) (at car4 loc1) (at car5 loc9) (at car6 loc1) (at car7 loc6) (at car8 loc8) (at car9 loc3) (at car10 loc4) (at car11 loc2) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc3) (at car2 loc3) (at car3 loc9) (at car4 loc8) (at car5 loc6) (at car6 loc8) (at car7 loc3) (at car8 loc5) (at car9 loc4) (at car10 loc2) (at car11 loc9)))
)
