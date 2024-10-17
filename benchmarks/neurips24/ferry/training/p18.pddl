(define (problem ferry_18-problem)
 (:domain ferry_18-domain)
 (:objects
   car1 car2 - car
   loc1 loc2 loc3 loc4 loc5 - location
 )
 (:init (at-ferry loc5) (at car1 loc3) (at car2 loc2) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc4) (at car2 loc3)))
)
