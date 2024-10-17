(define (problem p0_06)
 (:domain ferry)
 (:objects
   car1 car2 car3 car4 car5 - car
   loc1 loc2 loc3 loc4 loc5 loc6 - location
 )
 (:init (at-ferry loc3) (at car1 loc2) (at car2 loc2) (at car3 loc4) (at car4 loc2) (at car5 loc5) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc1) (at car2 loc4) (at car3 loc3) (at car4 loc5) (at car5 loc6)))
)
