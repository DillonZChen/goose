;; base case
;;
(define (problem ferry-04)
 (:domain ferry)
 (:objects 
    car1 car2 - car
    loc1 loc2 loc3 - location
 )
 (:init
    (empty-ferry)
    (at-ferry loc1)
    (at car1 loc1)
    (at car2 loc1)
)
 (:goal  (and
    (at car1 loc3)
    (at car2 loc3)
 )))
