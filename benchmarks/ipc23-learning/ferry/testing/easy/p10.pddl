;; cars=7, locations=8, out_folder=testing/easy, instance_id=10, seed=1016

(define (problem ferry-10)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 car7 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc6)
    (at car1 loc7)
    (at car2 loc6)
    (at car3 loc3)
    (at car4 loc2)
    (at car5 loc6)
    (at car6 loc8)
    (at car7 loc1)
)
 (:goal  (and (at car1 loc4)
   (at car2 loc5)
   (at car3 loc8)
   (at car4 loc5)
   (at car5 loc5)
   (at car6 loc5)
   (at car7 loc5))))
