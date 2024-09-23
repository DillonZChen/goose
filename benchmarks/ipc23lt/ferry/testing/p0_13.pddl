;; cars=9, locations=9, out_folder=testing/easy, instance_id=13, seed=1019

(define (problem ferry-13)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 car7 car8 car9 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc1)
    (at car1 loc6)
    (at car2 loc4)
    (at car3 loc2)
    (at car4 loc4)
    (at car5 loc8)
    (at car6 loc9)
    (at car7 loc4)
    (at car8 loc5)
    (at car9 loc7)
)
 (:goal  (and (at car1 loc8)
   (at car2 loc6)
   (at car3 loc6)
   (at car4 loc5)
   (at car5 loc5)
   (at car6 loc2)
   (at car7 loc7)
   (at car8 loc8)
   (at car9 loc1))))
