;; cars=9, locations=9, out_folder=training/easy, instance_id=51, seed=81

(define (problem ferry-51)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 car7 car8 car9 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc9)
    (at car1 loc8)
    (at car2 loc6)
    (at car3 loc9)
    (at car4 loc9)
    (at car5 loc7)
    (at car6 loc8)
    (at car7 loc1)
    (at car8 loc3)
    (at car9 loc3)
)
 (:goal  (and (at car1 loc9)
   (at car2 loc1)
   (at car3 loc6)
   (at car4 loc5)
   (at car5 loc6)
   (at car6 loc4)
   (at car7 loc6)
   (at car8 loc4)
   (at car9 loc2))))
