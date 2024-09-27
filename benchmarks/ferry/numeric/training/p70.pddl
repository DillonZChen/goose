(define (problem ferry_70-problem)
 (:domain ferry_70-domain)
 (:objects
   car1 car2 car3 car4 car5 car6 car7 car8 car9 car10 car11 car12 car13 car14 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 loc10 loc11 loc12 - location
 )
 (:init (at-ferry loc3) (at car1 loc8) (at car2 loc8) (at car3 loc3) (at car4 loc12) (at car5 loc7) (at car6 loc12) (at car7 loc6) (at car8 loc7) (at car9 loc9) (at car10 loc2) (at car11 loc9) (at car12 loc2) (at car13 loc2) (at car14 loc12) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc9) (at car2 loc5) (at car3 loc1) (at car4 loc11) (at car5 loc12) (at car6 loc4) (at car7 loc7) (at car8 loc4) (at car9 loc5) (at car10 loc5) (at car11 loc3) (at car12 loc4) (at car13 loc5) (at car14 loc6)))
)
