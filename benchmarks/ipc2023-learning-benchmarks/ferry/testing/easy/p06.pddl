;; cars=5, locations=6, out_folder=testing/easy, instance_id=6, seed=1012

(define (problem ferry-06)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 - car
    loc1 loc2 loc3 loc4 loc5 loc6 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc3)
    (at car1 loc2)
    (at car2 loc2)
    (at car3 loc4)
    (at car4 loc2)
    (at car5 loc5)
)
 (:goal  (and (at car1 loc1)
   (at car2 loc4)
   (at car3 loc3)
   (at car4 loc5)
   (at car5 loc6))))
