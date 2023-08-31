;; cars=2, locations=5, out_folder=training/easy, instance_id=18, seed=48

(define (problem ferry-18)
 (:domain ferry)
 (:objects 
    car1 car2 - car
    loc1 loc2 loc3 loc4 loc5 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc5)
    (at car1 loc3)
    (at car2 loc2)
)
 (:goal  (and (at car1 loc4)
   (at car2 loc3))))
