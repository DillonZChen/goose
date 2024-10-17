(define (problem p0_08)
 (:domain ferry)
 (:objects
   car1 car2 car3 car4 car5 car6 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 - location
 )
 (:init (at-ferry loc5) (at car1 loc1) (at car2 loc2) (at car3 loc3) (at car4 loc1) (at car5 loc4) (at car6 loc4) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc7) (at car2 loc6) (at car3 loc6) (at car4 loc3) (at car5 loc5) (at car6 loc6)))
)
