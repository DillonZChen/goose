;; cars=6, locations=8, out_folder=training/easy, instance_id=38, seed=68

(define (problem ferry-38)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc8)
    (at car1 loc2)
    (at car2 loc4)
    (at car3 loc7)
    (at car4 loc8)
    (at car5 loc5)
    (at car6 loc6)
)
 (:goal  (and (at car1 loc5)
   (at car2 loc6)
   (at car3 loc3)
   (at car4 loc2)
   (at car5 loc6)
   (at car6 loc7))))
