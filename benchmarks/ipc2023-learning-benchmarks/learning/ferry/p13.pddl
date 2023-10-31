;; cars=1, locations=5, out_folder=training/easy, instance_id=13, seed=43

(define (problem ferry-13)
 (:domain ferry)
 (:objects 
    car1 - car
    loc1 loc2 loc3 loc4 loc5 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc1)
    (at car1 loc3)
)
 (:goal  (and (at car1 loc2))))
