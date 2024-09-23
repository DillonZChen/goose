;; cars=3, locations=6, out_folder=training/easy, instance_id=24, seed=54

(define (problem ferry-24)
 (:domain ferry)
 (:objects 
    car1 car2 car3 - car
    loc1 loc2 loc3 loc4 loc5 loc6 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc2)
    (at car1 loc4)
    (at car2 loc5)
    (at car3 loc3)
)
 (:goal  (and (at car1 loc5)
   (at car2 loc4)
   (at car3 loc5))))
