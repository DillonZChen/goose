;; cars=2, locations=5, out_folder=testing/easy, instance_id=2, seed=1008

(define (problem ferry-02)
 (:domain ferry)
 (:objects 
    car1 car2 - car
    loc1 loc2 loc3 loc4 loc5 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc5)
    (at car1 loc4)
    (at car2 loc3)
)
 (:goal  (and (at car1 loc1)
   (at car2 loc1))))
