(define (problem ferry_17-problem)
 (:domain ferry_17-domain)
 (:objects
   car1 car2 - car
   loc1 loc2 loc3 loc4 loc5 - location
 )
 (:init (at-ferry loc3) (at car1 loc1) (at car2 loc4) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc5) (at car2 loc3)))
)
