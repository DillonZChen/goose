;; blocks=7, out_folder=testing/easy, instance_id=4, seed=1010

(define (problem blocksworld-04)
 (:domain blocksworld)
 (:objects b1 b2 b3 b4 b5 b6 b7 - object)
 (:init 
    (arm-empty)
    (clear b4)
    (on b4 b3)
    (on b3 b7)
    (on b7 b1)
    (on b1 b2)
    (on b2 b5)
    (on b5 b6)
    (on-table b6))
 (:goal  (and 
    (clear b4)
    (on b4 b1)
    (on b1 b2)
    (on b2 b3)
    (on b3 b5)
    (on b5 b6)
    (on b6 b7)
    (on-table b7))))
