;; cars=2, locations=5, out_folder=training/easy, instance_id=17, seed=47

(define (problem ferry-17)
 (:domain ferry)
 (:objects 
    car1 car2 - car
    loc1 loc2 loc3 loc4 loc5 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc3)
    (at car1 loc1)
    (at car2 loc4)
)
 (:goal  (and (at car1 loc5)
   (at car2 loc3))))
