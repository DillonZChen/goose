(define (problem ferry_77-problem)
 (:domain ferry_77-domain)
 (:objects
   car1 car2 car3 car4 car5 car6 car7 car8 car9 car10 car11 car12 car13 car14 car15 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 loc10 loc11 loc12 loc13 - location
 )
 (:init (at-ferry loc4) (at car1 loc10) (at car2 loc10) (at car3 loc13) (at car4 loc12) (at car5 loc11) (at car6 loc7) (at car7 loc8) (at car8 loc13) (at car9 loc5) (at car10 loc3) (at car11 loc7) (at car12 loc9) (at car13 loc13) (at car14 loc2) (at car15 loc5) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc5) (at car2 loc11) (at car3 loc1) (at car4 loc13) (at car5 loc12) (at car6 loc5) (at car7 loc6) (at car8 loc4) (at car9 loc3) (at car10 loc11) (at car11 loc4) (at car12 loc3) (at car13 loc12) (at car14 loc10) (at car15 loc3)))
)
