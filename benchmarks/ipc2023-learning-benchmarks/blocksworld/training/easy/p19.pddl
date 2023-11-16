;; blocks=6, out_folder=training/easy, instance_id=19, seed=46

(define (problem blocksworld-19)
 (:domain blocksworld)
 (:objects b1 b2 b3 b4 b5 b6 - object)
 (:init 
    (arm-empty)
    (clear b2)
    (on b2 b5)
    (on b5 b3)
    (on b3 b6)
    (on b6 b4)
    (on-table b4)
    (clear b1)
    (on-table b1))
 (:goal  (and 
    (clear b2)
    (on-table b2)
    (clear b5)
    (on b5 b4)
    (on b4 b3)
    (on b3 b6)
    (on b6 b1)
    (on-table b1))))
