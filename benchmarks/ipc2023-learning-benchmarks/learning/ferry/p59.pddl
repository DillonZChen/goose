;; cars=11, locations=10, out_folder=training/easy, instance_id=59, seed=89

(define (problem ferry-59)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 car7 car8 car9 car10 car11 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 loc10 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc2)
    (at car1 loc10)
    (at car2 loc5)
    (at car3 loc3)
    (at car4 loc6)
    (at car5 loc2)
    (at car6 loc3)
    (at car7 loc7)
    (at car8 loc6)
    (at car9 loc4)
    (at car10 loc5)
    (at car11 loc6)
)
 (:goal  (and (at car1 loc2)
   (at car2 loc3)
   (at car3 loc9)
   (at car4 loc1)
   (at car5 loc5)
   (at car6 loc4)
   (at car7 loc9)
   (at car8 loc1)
   (at car9 loc10)
   (at car10 loc7)
   (at car11 loc2))))
