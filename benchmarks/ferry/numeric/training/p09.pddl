(define (problem ferry_09-problem)
 (:domain ferry_09-domain)
 (:objects
   car1 car2 - car
   loc1 loc2 loc3 - location
 )
 (:init (at-ferry loc2) (at car1 loc3) (at car2 loc2) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc2) (at car2 loc3)))
)
