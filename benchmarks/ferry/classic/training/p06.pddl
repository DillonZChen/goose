;; base case
;;
(define (problem ferry-06)
 (:domain ferry)
 (:objects 
    car1 car2 - car
    loc1 loc2 loc3 - location
 )
 (:init
    (empty-ferry)
    (at-ferry loc3)
    (at car1 loc2)
    (at car2 loc2)
)
 (:goal  (and
    (at car1 loc1)
    (at car2 loc1)
 )))
