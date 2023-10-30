;; cars=8, locations=9, out_folder=training/easy, instance_id=47, seed=77

(define (problem ferry-47)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 car7 car8 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc5)
    (at car1 loc6)
    (at car2 loc4)
    (at car3 loc4)
    (at car4 loc4)
    (at car5 loc2)
    (at car6 loc5)
    (at car7 loc8)
    (at car8 loc9)
)
 (:goal  (and (at car1 loc4)
   (at car2 loc3)
   (at car3 loc1)
   (at car4 loc6)
   (at car5 loc1)
   (at car6 loc9)
   (at car7 loc3)
   (at car8 loc4))))
