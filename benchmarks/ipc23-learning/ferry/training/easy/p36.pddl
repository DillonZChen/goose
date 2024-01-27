;; cars=6, locations=8, out_folder=training/easy, instance_id=36, seed=66

(define (problem ferry-36)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc2)
    (at car1 loc5)
    (at car2 loc7)
    (at car3 loc4)
    (at car4 loc8)
    (at car5 loc5)
    (at car6 loc5)
)
 (:goal  (and (at car1 loc8)
   (at car2 loc8)
   (at car3 loc6)
   (at car4 loc1)
   (at car5 loc7)
   (at car6 loc8))))
