;; cars=4, locations=6, out_folder=testing/easy, instance_id=5, seed=1011

(define (problem ferry-05)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 - car
    loc1 loc2 loc3 loc4 loc5 loc6 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc3)
    (at car1 loc3)
    (at car2 loc1)
    (at car3 loc4)
    (at car4 loc2)
)
 (:goal  (and (at car1 loc5)
   (at car2 loc5)
   (at car3 loc5)
   (at car4 loc6))))
