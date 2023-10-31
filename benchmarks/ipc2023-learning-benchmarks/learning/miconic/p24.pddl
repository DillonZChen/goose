;; passengers=2, floors=5, out_folder=training/easy, instance_id=24, seed=51

(define (problem miconic-24)
 (:domain miconic)
 (:objects 
    p1 p2 - passenger
    f1 f2 f3 f4 f5 - floor
    )
 (:init 
    (lift-at f2)
    (origin p1 f5)
    (destin p1 f2)
    (origin p2 f2)
    (destin p2 f3)
    (above f1 f2)
    (above f1 f3)
    (above f1 f4)
    (above f1 f5)
    (above f2 f3)
    (above f2 f4)
    (above f2 f5)
    (above f3 f4)
    (above f3 f5)
    (above f4 f5)
)
 (:goal  (and (served p1)
   (served p2))))
