;; passengers=2, floors=6, out_folder=testing/easy, instance_id=6, seed=1012

(define (problem miconic-06)
 (:domain miconic)
 (:objects 
    p1 p2 - passenger
    f1 f2 f3 f4 f5 f6 - floor
    )
 (:init 
    (lift-at f3)
    (origin p1 f2)
    (destin p1 f3)
    (origin p2 f4)
    (destin p2 f2)
    (above f1 f2)
    (above f1 f3)
    (above f1 f4)
    (above f1 f5)
    (above f1 f6)
    (above f2 f3)
    (above f2 f4)
    (above f2 f5)
    (above f2 f6)
    (above f3 f4)
    (above f3 f5)
    (above f3 f6)
    (above f4 f5)
    (above f4 f6)
    (above f5 f6)
)
 (:goal  (and (served p1)
   (served p2))))
