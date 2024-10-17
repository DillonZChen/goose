(define (problem ferry_68-problem)
 (:domain ferry_68-domain)
 (:objects
   car1 car2 car3 car4 car5 car6 car7 car8 car9 car10 car11 car12 car13 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 loc10 loc11 loc12 - location
 )
 (:init (at-ferry loc6) (at car1 loc10) (at car2 loc1) (at car3 loc6) (at car4 loc7) (at car5 loc1) (at car6 loc8) (at car7 loc5) (at car8 loc12) (at car9 loc11) (at car10 loc9) (at car11 loc2) (at car12 loc8) (at car13 loc4) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc7) (at car2 loc2) (at car3 loc12) (at car4 loc6) (at car5 loc2) (at car6 loc1) (at car7 loc8) (at car8 loc5) (at car9 loc6) (at car10 loc1) (at car11 loc12) (at car12 loc9) (at car13 loc5)))
)
