;; cars=11, locations=10, out_folder=testing/easy, instance_id=16, seed=1022

(define (problem ferry-16)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 car7 car8 car9 car10 car11 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 loc10 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc3)
    (at car1 loc7)
    (at car2 loc10)
    (at car3 loc6)
    (at car4 loc2)
    (at car5 loc7)
    (at car6 loc9)
    (at car7 loc2)
    (at car8 loc6)
    (at car9 loc4)
    (at car10 loc7)
    (at car11 loc1)
)
 (:goal  (and (at car1 loc5)
   (at car2 loc8)
   (at car3 loc5)
   (at car4 loc8)
   (at car5 loc6)
   (at car6 loc2)
   (at car7 loc4)
   (at car8 loc8)
   (at car9 loc9)
   (at car10 loc1)
   (at car11 loc4))))
