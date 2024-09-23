;; cars=10, locations=10, out_folder=training/easy, instance_id=55, seed=85

(define (problem ferry-55)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 car7 car8 car9 car10 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 loc10 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc4)
    (at car1 loc10)
    (at car2 loc2)
    (at car3 loc6)
    (at car4 loc4)
    (at car5 loc4)
    (at car6 loc8)
    (at car7 loc6)
    (at car8 loc10)
    (at car9 loc4)
    (at car10 loc7)
)
 (:goal  (and (at car1 loc9)
   (at car2 loc10)
   (at car3 loc4)
   (at car4 loc9)
   (at car5 loc8)
   (at car6 loc2)
   (at car7 loc1)
   (at car8 loc9)
   (at car9 loc2)
   (at car10 loc8))))
