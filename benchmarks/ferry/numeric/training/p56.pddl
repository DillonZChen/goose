(define (problem ferry_56-problem)
 (:domain ferry_56-domain)
 (:objects
   car1 car2 car3 car4 car5 car6 car7 car8 car9 car10 car11 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 loc10 - location
 )
 (:init (at-ferry loc1) (at car1 loc9) (at car2 loc6) (at car3 loc1) (at car4 loc2) (at car5 loc9) (at car6 loc5) (at car7 loc7) (at car8 loc2) (at car9 loc9) (at car10 loc2) (at car11 loc7) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc8) (at car2 loc1) (at car3 loc5) (at car4 loc5) (at car5 loc6) (at car6 loc1) (at car7 loc3) (at car8 loc8) (at car9 loc5) (at car10 loc3) (at car11 loc9)))
)
