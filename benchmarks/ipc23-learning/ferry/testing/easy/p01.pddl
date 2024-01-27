;; cars=2, locations=5, out_folder=testing/easy, instance_id=1, seed=1007

(define (problem ferry-01)
 (:domain ferry)
 (:objects 
    car1 car2 - car
    loc1 loc2 loc3 loc4 loc5 - location
 )
 (:init 
    (empty-ferry)
    (at-ferry loc1)
    (at car1 loc5)
    (at car2 loc2)
)
 (:goal  (and (at car1 loc3)
   (at car2 loc3))))
