(define (problem p0_04)
 (:domain ferry)
 (:objects
   car1 car2 car3 - car
   loc1 loc2 loc3 loc4 loc5 loc6 - location
 )
 (:init (at-ferry loc6) (at car1 loc5) (at car2 loc2) (at car3 loc5) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc1) (at car2 loc5) (at car3 loc4)))
)
