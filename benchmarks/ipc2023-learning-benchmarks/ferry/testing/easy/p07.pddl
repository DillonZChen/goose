;; cars=5, locations=7, out_folder=testing/easy, instance_id=7, seed=1013

(define (problem ferry-07)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc4)
    (at car1 loc7)
    (at car2 loc3)
    (at car3 loc6)
    (at car4 loc7)
    (at car5 loc6)
)
 (:goal  (and (at car1 loc6)
   (at car2 loc2)
   (at car3 loc5)
   (at car4 loc5)
   (at car5 loc1))))
