;; cars=1, locations=5, out_folder=training/easy, instance_id=16, seed=46

(define (problem ferry-16)
 (:domain ferry)
 (:objects 
    car1 - car
    loc1 loc2 loc3 loc4 loc5 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc1)
    (at car1 loc4)
)
 (:goal  (and (at car1 loc1))))
