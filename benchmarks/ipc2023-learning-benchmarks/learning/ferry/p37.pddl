;; cars=6, locations=8, out_folder=training/easy, instance_id=37, seed=67

(define (problem ferry-37)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc2)
    (at car1 loc2)
    (at car2 loc7)
    (at car3 loc8)
    (at car4 loc7)
    (at car5 loc5)
    (at car6 loc7)
)
 (:goal  (and (at car1 loc5)
   (at car2 loc5)
   (at car3 loc3)
   (at car4 loc2)
   (at car5 loc7)
   (at car6 loc1))))
