(define (problem ferry_21-problem)
 (:domain ferry_21-domain)
 (:objects
   car1 car2 car3 - car
   loc1 loc2 loc3 loc4 loc5 loc6 - location
 )
 (:init (at-ferry loc2) (at car1 loc5) (at car2 loc5) (at car3 loc2) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc2) (at car2 loc2) (at car3 loc6)))
)
