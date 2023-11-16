;; cars=7, locations=7, out_folder=testing/easy, instance_id=9, seed=1015

(define (problem ferry-09)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 car7 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc1)
    (at car1 loc7)
    (at car2 loc3)
    (at car3 loc4)
    (at car4 loc5)
    (at car5 loc6)
    (at car6 loc7)
    (at car7 loc7)
)
 (:goal  (and (at car1 loc6)
   (at car2 loc5)
   (at car3 loc3)
   (at car4 loc4)
   (at car5 loc5)
   (at car6 loc6)
   (at car7 loc3))))
