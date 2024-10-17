(define (problem ferry_13-problem)
 (:domain ferry_13-domain)
 (:objects
   car1 - car
   loc1 loc2 loc3 loc4 loc5 - location
 )
 (:init (at-ferry loc1) (at car1 loc3) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc2)))
)
