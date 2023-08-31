;; cars=10, locations=9, out_folder=testing/easy, instance_id=14, seed=1020

(define (problem ferry-14)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 car7 car8 car9 car10 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc2)
    (at car1 loc1)
    (at car2 loc8)
    (at car3 loc7)
    (at car4 loc3)
    (at car5 loc5)
    (at car6 loc3)
    (at car7 loc5)
    (at car8 loc6)
    (at car9 loc7)
    (at car10 loc9)
)
 (:goal  (and (at car1 loc6)
   (at car2 loc5)
   (at car3 loc8)
   (at car4 loc1)
   (at car5 loc9)
   (at car6 loc4)
   (at car7 loc1)
   (at car8 loc9)
   (at car9 loc3)
   (at car10 loc7))))
