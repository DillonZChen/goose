(define (problem ferry_43-problem)
 (:domain ferry_43-domain)
 (:objects
   car1 car2 car3 car4 car5 car6 car7 car8 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 - location
 )
 (:init (at-ferry loc5) (at car1 loc2) (at car2 loc8) (at car3 loc3) (at car4 loc8) (at car5 loc5) (at car6 loc2) (at car7 loc8) (at car8 loc7) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc6) (at car2 loc5) (at car3 loc2) (at car4 loc4) (at car5 loc1) (at car6 loc1) (at car7 loc3) (at car8 loc4)))
)
