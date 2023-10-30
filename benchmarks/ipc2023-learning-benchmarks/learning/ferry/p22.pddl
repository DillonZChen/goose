;; cars=3, locations=6, out_folder=training/easy, instance_id=22, seed=52

(define (problem ferry-22)
 (:domain ferry)
 (:objects 
    car1 car2 car3 - car
    loc1 loc2 loc3 loc4 loc5 loc6 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc3)
    (at car1 loc1)
    (at car2 loc6)
    (at car3 loc5)
)
 (:goal  (and (at car1 loc5)
   (at car2 loc3)
   (at car3 loc4))))
