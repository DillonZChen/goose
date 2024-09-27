;; cars=9, locations=9, out_folder=training/easy, instance_id=50, seed=80

(define (problem ferry-50)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 car7 car8 car9 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc5)
    (at car1 loc7)
    (at car2 loc9)
    (at car3 loc7)
    (at car4 loc6)
    (at car5 loc6)
    (at car6 loc9)
    (at car7 loc7)
    (at car8 loc6)
    (at car9 loc1)
)
 (:goal  (and (at car1 loc8)
   (at car2 loc4)
   (at car3 loc1)
   (at car4 loc8)
   (at car5 loc4)
   (at car6 loc3)
   (at car7 loc1)
   (at car8 loc3)
   (at car9 loc9))))
