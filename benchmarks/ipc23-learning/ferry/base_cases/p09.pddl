;; base case
;;
(define (problem ferry-09)
 (:domain ferry)
 (:objects 
    car1 car2 - car
    loc1 loc2 loc3 - location
 )
 (:init
    (empty-ferry)
    (at-ferry loc2)
    (at car1 loc3)
    (at car2 loc2)
)
 (:goal  (and
    (at car1 loc2)
    (at car2 loc3)
 )))
