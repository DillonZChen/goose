;; cars=3, locations=6, out_folder=testing/easy, instance_id=4, seed=1010

(define (problem ferry-04)
 (:domain ferry)
 (:objects 
    car1 car2 car3 - car
    loc1 loc2 loc3 loc4 loc5 loc6 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc6)
    (at car1 loc5)
    (at car2 loc2)
    (at car3 loc5)
)
 (:goal  (and (at car1 loc1)
   (at car2 loc5)
   (at car3 loc4))))
