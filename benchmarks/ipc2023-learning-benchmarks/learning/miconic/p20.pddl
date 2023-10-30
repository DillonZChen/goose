;; passengers=1, floors=5, out_folder=training/easy, instance_id=20, seed=47

(define (problem miconic-20)
 (:domain miconic)
 (:objects 
    p1 - passenger
    f1 f2 f3 f4 f5 - floor
    )
 (:init 
    (lift-at f3)
    (origin p1 f1)
    (destin p1 f5)
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
 (:goal  (and (served p1))))
