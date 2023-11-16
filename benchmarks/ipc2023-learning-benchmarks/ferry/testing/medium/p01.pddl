;; cars=10, locations=20, out_folder=testing/medium, instance_id=1, seed=1007

(define (problem ferry-01)
 (:domain ferry)
 (:objects 
    car1 car2 car3 car4 car5 car6 car7 car8 car9 car10 - car
    loc1 loc2 loc3 loc4 loc5 loc6 loc7 loc8 loc9 loc10 loc11 loc12 loc13 loc14 loc15 loc16 loc17 loc18 loc19 loc20 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc2)
    (at car1 loc20)
    (at car2 loc6)
    (at car3 loc10)
    (at car4 loc6)
    (at car5 loc8)
    (at car6 loc17)
    (at car7 loc2)
    (at car8 loc7)
    (at car9 loc18)
    (at car10 loc2)
)
 (:goal  (and (at car1 loc15)
   (at car2 loc20)
   (at car3 loc11)
   (at car4 loc2)
   (at car5 loc2)
   (at car6 loc14)
   (at car7 loc8)
   (at car8 loc9)
   (at car9 loc3)
   (at car10 loc5))))
