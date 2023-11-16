;; cars=7, locations=8, out_folder=training/easy, instance_id=42, seed=72

(define (problem ferry-42)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 car7 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc2)
    (at car1 loc3)
    (at car2 loc6)
    (at car3 loc6)
    (at car4 loc5)
    (at car5 loc3)
    (at car6 loc6)
    (at car7 loc4)
)
 (:goal  (and (at car1 loc5)
   (at car2 loc2)
   (at car3 loc7)
   (at car4 loc1)
   (at car5 loc7)
   (at car6 loc8)
   (at car7 loc2))))
