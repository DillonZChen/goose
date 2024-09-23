;; cars=2, locations=6, out_folder=training/easy, instance_id=20, seed=50

(define (problem ferry-20)
 (:domain ferry)
 (:objects 
    car1 car2 - car
    loc1 loc2 loc3 loc4 loc5 loc6 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc4)
    (at car1 loc3)
    (at car2 loc3)
)
 (:goal  (and (at car1 loc2)
   (at car2 loc5))))
