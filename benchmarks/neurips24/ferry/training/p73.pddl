(define (problem ferry_73-problem)
 (:domain ferry_73-domain)
 (:objects
   car1 car2 car3 car4 car5 car6 car7 car8 car9 car10 car11 car12 car13 car14 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 loc10 loc11 loc12 - location
 )
 (:init (at-ferry loc12) (at car1 loc8) (at car2 loc12) (at car3 loc12) (at car4 loc12) (at car5 loc4) (at car6 loc12) (at car7 loc2) (at car8 loc10) (at car9 loc12) (at car10 loc2) (at car11 loc11) (at car12 loc7) (at car13 loc8) (at car14 loc6) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc12) (at car2 loc2) (at car3 loc8) (at car4 loc2) (at car5 loc3) (at car6 loc4) (at car7 loc11) (at car8 loc1) (at car9 loc1) (at car10 loc5) (at car11 loc2) (at car12 loc3) (at car13 loc11) (at car14 loc10)))
)
