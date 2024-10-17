(define (problem ferry_04-problem)
 (:domain ferry_04-domain)
 (:objects
   car1 car2 - car
   loc1 loc2 loc3 - location
 )
 (:init (at-ferry loc1) (at car1 loc1) (at car2 loc1) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc3) (at car2 loc3)))
)
