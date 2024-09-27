;; blocks=6, out_folder=testing/easy, instance_id=3, seed=1009

(define (problem blocksworld-03)
 (:domain blocksworld)
 (:objects b1 b2 b3 b4 b5 b6 - object)
 (:init 
    (arm-empty)
    (clear b3)
    (on b3 b6)
    (on b6 b5)
    (on b5 b4)
    (on b4 b1)
    (on b1 b2)
    (on-table b2))
 (:goal  (and 
    (clear b4)
    (on b4 b3)
    (on b3 b5)
    (on b5 b2)
    (on b2 b6)
    (on b6 b1)
    (on-table b1))))
