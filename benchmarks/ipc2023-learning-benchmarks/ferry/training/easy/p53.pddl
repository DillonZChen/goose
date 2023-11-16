;; cars=10, locations=10, out_folder=training/easy, instance_id=53, seed=83

(define (problem ferry-53)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 car7 car8 car9 car10 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 loc10 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc8)
    (at car1 loc8)
    (at car2 loc2)
    (at car3 loc3)
    (at car4 loc7)
    (at car5 loc1)
    (at car6 loc1)
    (at car7 loc4)
    (at car8 loc4)
    (at car9 loc3)
    (at car10 loc5)
)
 (:goal  (and (at car1 loc2)
   (at car2 loc1)
   (at car3 loc8)
   (at car4 loc3)
   (at car5 loc7)
   (at car6 loc6)
   (at car7 loc10)
   (at car8 loc5)
   (at car9 loc9)
   (at car10 loc4))))
