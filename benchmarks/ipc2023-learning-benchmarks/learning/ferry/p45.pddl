;; cars=8, locations=9, out_folder=training/easy, instance_id=45, seed=75

(define (problem ferry-45)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 car7 car8 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc8)
    (at car1 loc7)
    (at car2 loc8)
    (at car3 loc1)
    (at car4 loc9)
    (at car5 loc8)
    (at car6 loc6)
    (at car7 loc2)
    (at car8 loc6)
)
 (:goal  (and (at car1 loc2)
   (at car2 loc5)
   (at car3 loc6)
   (at car4 loc8)
   (at car5 loc6)
   (at car6 loc9)
   (at car7 loc6)
   (at car8 loc8))))
