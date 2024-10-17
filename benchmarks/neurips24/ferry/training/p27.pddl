(define (problem ferry_27-problem)
 (:domain ferry_27-domain)
 (:objects
   car1 car2 car3 car4 - car
   loc1 loc2 loc3 loc4 loc5 loc6 - location
 )
 (:init (at-ferry loc1) (at car1 loc3) (at car2 loc5) (at car3 loc5) (at car4 loc1) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc2) (at car2 loc6) (at car3 loc3) (at car4 loc5)))
)
