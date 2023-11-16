;; cars=1, locations=5, out_folder=training/easy, instance_id=12, seed=42

(define (problem ferry-12)
 (:domain ferry)
 (:objects 
    car1 - car
    loc1 loc2 loc3 loc4 loc5 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc1)
    (at car1 loc1)
)
 (:goal  (and (at car1 loc4))))
