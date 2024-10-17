;; cars=1, locations=5, out_folder=training/easy, instance_id=14, seed=44

(define (problem ferry-14)
 (:domain ferry)
 (:objects 
    car1 - car
    loc1 loc2 loc3 loc4 loc5 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc4)
    (at car1 loc5)
)
 (:goal  (and (at car1 loc1))))
