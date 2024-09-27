(define (problem ferry_81-problem)
 (:domain ferry_81-domain)
 (:objects
   car1 car2 car3 car4 car5 car6 car7 car8 car9 car10 car11 car12 car13 car14 car15 car16 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 loc10 loc11 loc12 loc13 - location
 )
 (:init (at-ferry loc4) (at car1 loc6) (at car2 loc8) (at car3 loc4) (at car4 loc7) (at car5 loc7) (at car6 loc10) (at car7 loc13) (at car8 loc3) (at car9 loc11) (at car10 loc4) (at car11 loc12) (at car12 loc7) (at car13 loc9) (at car14 loc4) (at car15 loc3) (at car16 loc11) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc9) (at car2 loc7) (at car3 loc8) (at car4 loc13) (at car5 loc5) (at car6 loc7) (at car7 loc4) (at car8 loc13) (at car9 loc13) (at car10 loc7) (at car11 loc8) (at car12 loc1) (at car13 loc6) (at car14 loc12) (at car15 loc12) (at car16 loc4)))
)
