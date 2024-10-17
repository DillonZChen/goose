(define (problem ferry_02-problem)
 (:domain ferry_02-domain)
 (:objects
   car1 - car
   loc1 loc2 - location
 )
 (:init (at-ferry loc2) (at car1 loc1) (= (ferry-capacity) 4))
 (:goal (and (at car1 loc2)))
)
