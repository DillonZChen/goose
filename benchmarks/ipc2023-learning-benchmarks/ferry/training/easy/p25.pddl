;; cars=3, locations=6, out_folder=training/easy, instance_id=25, seed=55

(define (problem ferry-25)
 (:domain ferry)
 (:objects 
    car1 car2 car3 - car
    loc1 loc2 loc3 loc4 loc5 loc6 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc1)
    (at car1 loc2)
    (at car2 loc2)
    (at car3 loc6)
)
 (:goal  (and (at car1 loc4)
   (at car2 loc1)
   (at car3 loc2))))
