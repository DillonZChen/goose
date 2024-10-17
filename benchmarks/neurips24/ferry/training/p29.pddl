(define (problem ferry_29-problem)
 (:domain ferry_29-domain)
 (:objects
   car1 car2 car3 car4 - car
   loc1 loc2 loc3 loc4 loc5 loc6 loc7 - location
 )
 (:init (at-ferry loc2) (at car1 loc1) (at car2 loc6) (at car3 loc4) (at car4 loc1) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc3) (at car2 loc3) (at car3 loc7) (at car4 loc2)))
)
