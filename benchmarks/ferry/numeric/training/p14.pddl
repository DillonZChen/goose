(define (problem ferry_14-problem)
 (:domain ferry_14-domain)
 (:objects
   car1 - car
   loc1 loc2 loc3 loc4 loc5 - location
 )
 (:init (at-ferry loc4) (at car1 loc5) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc1)))
)
