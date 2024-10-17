(define (problem ferry_44-problem)
 (:domain ferry_44-domain)
 (:objects
   car1 car2 car3 car4 car5 car6 car7 car8 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 - location
 )
 (:init (at-ferry loc9) (at car1 loc2) (at car2 loc6) (at car3 loc2) (at car4 loc5) (at car5 loc3) (at car6 loc4) (at car7 loc6) (at car8 loc2) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc9) (at car2 loc9) (at car3 loc6) (at car4 loc9) (at car5 loc4) (at car6 loc7) (at car7 loc5) (at car8 loc1)))
)
