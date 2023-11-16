;; cars=11, locations=10, out_folder=training/easy, instance_id=57, seed=87

(define (problem ferry-57)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 car7 car8 car9 car10 car11 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 loc10 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc3)
    (at car1 loc4)
    (at car2 loc9)
    (at car3 loc2)
    (at car4 loc5)
    (at car5 loc6)
    (at car6 loc9)
    (at car7 loc6)
    (at car8 loc2)
    (at car9 loc6)
    (at car10 loc1)
    (at car11 loc5)
)
 (:goal  (and (at car1 loc8)
   (at car2 loc2)
   (at car3 loc9)
   (at car4 loc10)
   (at car5 loc2)
   (at car6 loc4)
   (at car7 loc5)
   (at car8 loc4)
   (at car9 loc10)
   (at car10 loc3)
   (at car11 loc6))))
