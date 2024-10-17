;; passengers=1, floors=4, out_folder=training/easy, instance_id=16, seed=43

(define (problem miconic-16)
 (:domain miconic)
 (:objects 
    p1 - passenger
    f1 f2 f3 f4 - floor
    )
 (:init 
    (lift-at f1)
    (origin p1 f3)
    (destin p1 f4)
    (above f1 f2)
    (above f1 f3)
    (above f1 f4)
    (above f2 f3)
    (above f2 f4)
    (above f3 f4)
)
 (:goal  (and (served p1))))
